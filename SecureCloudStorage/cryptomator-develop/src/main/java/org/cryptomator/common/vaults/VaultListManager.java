/*******************************************************************************
 * Copyright (c) 2016, 2017 Sebastian Stenzel and others.
 * All rights reserved.
 * This program and the accompanying materials are made available under the terms of the accompanying LICENSE file.
 *
 * Contributors:
 *     Sebastian Stenzel - initial API and implementation
 *******************************************************************************/
package org.cryptomator.common.vaults;

import org.cryptomator.common.settings.Settings;
import org.cryptomator.common.settings.VaultSettings;
import org.cryptomator.common.vaults.VaultState.Value;
import org.cryptomator.cryptofs.CryptoFileSystemProvider;
import org.cryptomator.cryptofs.DirStructure;
import org.cryptomator.cryptofs.migration.Migrators;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.inject.Inject;
import javax.inject.Singleton;
import javafx.collections.ObservableList;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.NoSuchFileException;
import java.nio.file.Path;
import java.util.Collection;
import java.util.Optional;
import java.util.ResourceBundle;

import static org.cryptomator.common.Constants.MASTERKEY_FILENAME;
import static org.cryptomator.common.Constants.VAULTCONFIG_FILENAME;
import static org.cryptomator.common.vaults.VaultState.Value.ERROR;
import static org.cryptomator.common.vaults.VaultState.Value.LOCKED;

@Singleton
public class VaultListManager {

	private static final Logger LOG = LoggerFactory.getLogger(VaultListManager.class);

	private final AutoLocker autoLocker;
	private final VaultComponent.Builder vaultComponentBuilder;
	private final ObservableList<Vault> vaultList;
	private final String defaultVaultName;

	@Inject
	public VaultListManager(ObservableList<Vault> vaultList, AutoLocker autoLocker, VaultComponent.Builder vaultComponentBuilder, ResourceBundle resourceBundle, Settings settings) {
		this.vaultList = vaultList;
		this.autoLocker = autoLocker;
		this.vaultComponentBuilder = vaultComponentBuilder;
		this.defaultVaultName = resourceBundle.getString("defaults.vault.vaultName");

		addAll(settings.getDirectories());
		vaultList.addListener(new VaultListChangeListener(settings.getDirectories()));
		autoLocker.init();
	}

	public Vault add(Path pathToVault) throws IOException {
		Path normalizedPathToVault = pathToVault.normalize().toAbsolutePath();
		if (CryptoFileSystemProvider.checkDirStructureForVault(normalizedPathToVault, VAULTCONFIG_FILENAME, MASTERKEY_FILENAME) == DirStructure.UNRELATED) {
			throw new NoSuchFileException(normalizedPathToVault.toString(), null, "Not a vault directory");
		}
		
		return get(normalizedPathToVault) //
				.orElseGet(() -> {
					Vault newVault = create(newVaultSettings(normalizedPathToVault));
					vaultList.add(newVault);
					return newVault;
				});
	}

	
	//////////// Duplicated functions for secube /////////////////
	
	/*
	 * Creation of the vault using SEcube with the Vault ID
	 * */
	
	public Vault addSEcube(Path pathToVault, String id) throws IOException {
		Path normalizedPathToVault = pathToVault.normalize().toAbsolutePath();
		if (CryptoFileSystemProvider.checkDirStructureForVault(normalizedPathToVault, VAULTCONFIG_FILENAME, MASTERKEY_FILENAME) == DirStructure.UNRELATED) {
			throw new NoSuchFileException(normalizedPathToVault.toString(), null, "Not a vault directory");
		}

		return get(normalizedPathToVault) //
				.orElseGet(() -> {
					Vault newVault = createSEcube(newVaultSettingsSEcube(normalizedPathToVault, id));
					// Set value of vaultsecube true to label the vault as "SECUBE" Vault type
					newVault.setVaultsecube();
					// vault is added to the list of Vaults
					vaultList.add(newVault);
					return newVault;
				});
	}
	
	private VaultSettings newVaultSettingsSEcube(Path path, String id) {
		VaultSettings vaultSettings = VaultSettings.setId(id);
		vaultSettings.path().set(path);
		if (path.getFileName() != null) {
			vaultSettings.displayName().set(path.getFileName().toString());
		} else {
			vaultSettings.displayName().set(defaultVaultName);
		}
		return vaultSettings;
	}
	////////////////////////////////////////////////////////////////
	
	private VaultSettings newVaultSettings(Path path) {
		VaultSettings vaultSettings = VaultSettings.withRandomId();
		vaultSettings.path().set(path);
		if (path.getFileName() != null) {
			vaultSettings.displayName().set(path.getFileName().toString());
		} else {
			vaultSettings.displayName().set(defaultVaultName);
		}
		return vaultSettings;
	}

	private void addAll(Collection<VaultSettings> vaultSettings) {
		Collection<Vault> vaults = vaultSettings.stream().map(this::create).toList();
		vaultList.addAll(vaults);
	}

	private Optional<Vault> get(Path vaultPath) {
		assert vaultPath.isAbsolute();
		assert vaultPath.normalize().equals(vaultPath);
		return vaultList.stream() //
				.filter(v -> vaultPath.equals(v.getPath())) //
				.findAny();
	}

	private Vault create(VaultSettings vaultSettings) {
		VaultComponent.Builder compBuilder = vaultComponentBuilder.vaultSettings(vaultSettings);
		try {
			VaultState.Value vaultState; 
			/*Depending on the Vault type one of those two function are called 
			 * to determine and assign the state of the vault.
			 */
			if (vaultSettings.getVaultsecube()) {
				 vaultState = determineVaultStateSEcube(vaultSettings.path().get());

			} else {
				 vaultState = determineVaultState(vaultSettings.path().get());
			}
			VaultConfigCache wrapper = new VaultConfigCache(vaultSettings);
			compBuilder.vaultConfigCache(wrapper); //first set the wrapper in the builder, THEN try to load config
			if (vaultState == LOCKED || vaultState == Value.SECUBE) { //for legacy reasons: pre v8 vault do not have a config, but they are in the NEEDS_MIGRATION state
				wrapper.reloadConfig();
			}
			compBuilder.initialVaultState(vaultState);
		} catch (IOException e) {
			LOG.warn("Failed to determine vault state for " + vaultSettings.path().get(), e);
			compBuilder.initialVaultState(ERROR);
			compBuilder.initialErrorCause(e);
		}
		return compBuilder.build().vault();
	}
	
	/*This function is called when the Vault is created.
	 * It calls the determinseVaultState function to assign the Vault state
	 * */
	private Vault createSEcube(VaultSettings vaultSettings) {
		VaultComponent.Builder compBuilder = vaultComponentBuilder.vaultSettings(vaultSettings);
		try {
			VaultState.Value vaultState = determineVaultStateSEcube(vaultSettings.path().get());
			VaultConfigCache wrapper = new VaultConfigCache(vaultSettings);
			compBuilder.vaultConfigCache(wrapper); //first set the wrapper in the builder, THEN try to load config
			if (vaultState == Value.SECUBE) { //for legacy reasons: pre v8 vault do not have a config, but they are in the NEEDS_MIGRATION state
				vaultSettings.setVaultsecube();
				wrapper.reloadConfig();
			}
			compBuilder.initialVaultState(vaultState);
		} catch (IOException e) {
			LOG.warn("Failed to determine vault state for " + vaultSettings.path().get(), e);
			compBuilder.initialVaultState(ERROR);
			compBuilder.initialErrorCause(e);
		}
		return compBuilder.build().vault();
	}

	
	/* This function is called when the state should be redetermined. 
	 * The case SECUBE state has been added.*/
	public static VaultState.Value redetermineVaultState(Vault vault) {
		
		VaultState state = vault.stateProperty();
		VaultState.Value previousState = state.getValue();
		return switch (previousState) {
			case LOCKED, NEEDS_MIGRATION, MISSING -> {
				try {
					var determinedState = determineVaultState(vault.getPath());
					if (determinedState == LOCKED) {
						vault.getVaultConfigCache().reloadConfig();
					}
					state.set(determinedState);
					yield determinedState;
				} catch (IOException e) {
					LOG.warn("Failed to determine vault state for " + vault.getPath(), e);
					state.set(ERROR);
					vault.setLastKnownException(e);
					yield ERROR;
				}
			}
			case ERROR, UNLOCKED, PROCESSING -> previousState;
			
			case SECUBE -> {
				try {
					var determinedState = determineVaultStateSEcube(vault.getPath());
					if (determinedState == Value.SECUBE) {
						vault.getVaultConfigCache().reloadConfig();
					}
					state.set(determinedState);
					yield determinedState;
				} catch (IOException e) {
					LOG.warn("Failed to determine vault state for " + vault.getPath(), e);
					state.set(ERROR);
					vault.setLastKnownException(e);
					yield ERROR;
				}

			}
		default -> throw new IllegalArgumentException("Unexpected value: " + previousState);
		};
	}

	/* This function will assign the state SECUBE to the Vault*/
	private static VaultState.Value determineVaultStateSEcube(Path pathToVault) throws IOException {

		if (!Files.exists(pathToVault)) {
			return VaultState.Value.MISSING;
		}
		return switch (CryptoFileSystemProvider.checkDirStructureForVault(pathToVault, VAULTCONFIG_FILENAME, MASTERKEY_FILENAME)) {
			case VAULT -> VaultState.Value.SECUBE;
			case UNRELATED -> VaultState.Value.MISSING;
			case MAYBE_LEGACY -> Migrators.get().needsMigration(pathToVault, VAULTCONFIG_FILENAME, MASTERKEY_FILENAME) ? //
					VaultState.Value.NEEDS_MIGRATION //
					: VaultState.Value.MISSING;
		};
	}
	
	private static VaultState.Value determineVaultState(Path pathToVault) throws IOException {
		if (!Files.exists(pathToVault)) {
			return VaultState.Value.MISSING;
		}
		return switch (CryptoFileSystemProvider.checkDirStructureForVault(pathToVault, VAULTCONFIG_FILENAME, MASTERKEY_FILENAME)) {
			case VAULT -> VaultState.Value.LOCKED;
			case UNRELATED -> VaultState.Value.MISSING;
			case MAYBE_LEGACY -> Migrators.get().needsMigration(pathToVault, VAULTCONFIG_FILENAME, MASTERKEY_FILENAME) ? //
					VaultState.Value.NEEDS_MIGRATION //
					: VaultState.Value.MISSING;
		};
	}

}