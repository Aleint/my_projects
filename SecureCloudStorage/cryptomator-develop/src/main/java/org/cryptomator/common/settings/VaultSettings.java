/*******************************************************************************
 * Copyright (c) 2017 Skymatic UG (haftungsbeschränkt).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the accompanying LICENSE file.
 *******************************************************************************/
package org.cryptomator.common.settings;

import com.google.common.base.CharMatcher;
import com.google.common.base.Strings;
import com.google.common.io.BaseEncoding;

import javafx.beans.Observable;
import javafx.beans.binding.Bindings;
import javafx.beans.binding.StringBinding;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import java.nio.file.Path;
import java.util.Objects;
import java.util.Optional;
import java.util.Random;

/**
 * The settings specific to a single vault.
 */
public class VaultSettings {

	public static final boolean DEFAULT_UNLOCK_AFTER_STARTUP = false;
	public static final boolean DEFAULT_REVEAL_AFTER_MOUNT = true;
	public static final boolean DEFAULT_USES_INDIVIDUAL_MOUNTPATH = false;
	public static final boolean DEFAULT_USES_READONLY_MODE = false;
	public static final String DEFAULT_MOUNT_FLAGS = "";
	public static final int DEFAULT_MAX_CLEARTEXT_FILENAME_LENGTH = -1;
	public static final WhenUnlocked DEFAULT_ACTION_AFTER_UNLOCK = WhenUnlocked.ASK;
	public static final boolean DEFAULT_AUTOLOCK_WHEN_IDLE = false;
	public static final int DEFAULT_AUTOLOCK_IDLE_SECONDS = 30 * 60;

	private static final Random RNG = new Random();

	
	
	private final String id;
	/*Added the boolean vaultsecube
	 * This value keep track of the Vault type.
	 */
	private boolean vaultsecube = false;
	private final ObjectProperty<Path> path = new SimpleObjectProperty<>();
	private final StringProperty displayName = new SimpleStringProperty();
	private final StringProperty winDriveLetter = new SimpleStringProperty();
	private final BooleanProperty unlockAfterStartup = new SimpleBooleanProperty(DEFAULT_UNLOCK_AFTER_STARTUP);
	private final BooleanProperty revealAfterMount = new SimpleBooleanProperty(DEFAULT_REVEAL_AFTER_MOUNT);
	private final BooleanProperty useCustomMountPath = new SimpleBooleanProperty(DEFAULT_USES_INDIVIDUAL_MOUNTPATH);
	private final StringProperty customMountPath = new SimpleStringProperty();
	private final BooleanProperty usesReadOnlyMode = new SimpleBooleanProperty(DEFAULT_USES_READONLY_MODE);
	private final StringProperty mountFlags = new SimpleStringProperty(DEFAULT_MOUNT_FLAGS);
	private final IntegerProperty maxCleartextFilenameLength = new SimpleIntegerProperty(DEFAULT_MAX_CLEARTEXT_FILENAME_LENGTH);
	private final ObjectProperty<WhenUnlocked> actionAfterUnlock = new SimpleObjectProperty<>(DEFAULT_ACTION_AFTER_UNLOCK);
	private final BooleanProperty autoLockWhenIdle = new SimpleBooleanProperty(DEFAULT_AUTOLOCK_WHEN_IDLE);
	private final IntegerProperty autoLockIdleSeconds = new SimpleIntegerProperty(DEFAULT_AUTOLOCK_IDLE_SECONDS);
	private final StringBinding mountName;

	public VaultSettings(String id) {
		this.id = Objects.requireNonNull(id);
		this.mountName = Bindings.createStringBinding(this::normalizeDisplayName, displayName);
	}

	Observable[] observables() {
		return new Observable[]{path, displayName, winDriveLetter, unlockAfterStartup, revealAfterMount, useCustomMountPath, customMountPath, usesReadOnlyMode, mountFlags, maxCleartextFilenameLength, actionAfterUnlock, autoLockWhenIdle, autoLockIdleSeconds};
	}
	
	/* GETTER and SETTER of the boolean vaultsecube variable. */
	public void setVaultsecube() {
		this.vaultsecube=true;
	}
	
	public boolean getVaultsecube() {
		return this.vaultsecube;
	}
	
	//////////////////////////////////////////////////////////////
	
	public static VaultSettings withRandomId() {
		return new VaultSettings(generateId());
	}

	private static String generateId() {
		byte[] randomBytes = new byte[9];
		RNG.nextBytes(randomBytes);
		return BaseEncoding.base64Url().encode(randomBytes);
	}

	//visible for testing
	String normalizeDisplayName() {
		var original = displayName.getValueSafe();
		if (original.isBlank() || ".".equals(original) || "..".equals(original)) {
			return "_";
		}

		// replace whitespaces (tabs, linebreaks, ...) by simple space (0x20)
		var withoutFancyWhitespaces = CharMatcher.whitespace().collapseFrom(original, ' ');

		// replace control chars as well as chars that aren't allowed in file names on standard file systems by underscore
		return CharMatcher.anyOf("<>:\"/\\|?*").or(CharMatcher.javaIsoControl()).collapseFrom(withoutFancyWhitespaces, '_');
	}

	/* Getter/Setter */
	public static VaultSettings setId(String id) {
		return new VaultSettings(id);
	}

	public String getId() {
		return id;
	}

	public ObjectProperty<Path> path() {
		return path;
	}

	public StringProperty displayName() {
		return displayName;
	}

	public StringBinding mountName() {
		return mountName;
	}

	public StringProperty winDriveLetter() {
		return winDriveLetter;
	}

	public Optional<String> getWinDriveLetter() {
		String current = this.winDriveLetter.get();
		if (!Strings.isNullOrEmpty(current)) {
			return Optional.of(current);
		}
		return Optional.empty();
	}

	public BooleanProperty unlockAfterStartup() {
		return unlockAfterStartup;
	}

	public BooleanProperty revealAfterMount() {
		return revealAfterMount;
	}

	public BooleanProperty useCustomMountPath() {
		return useCustomMountPath;
	}

	public StringProperty customMountPath() {
		return customMountPath;
	}

	public Optional<String> getCustomMountPath() {
		if (useCustomMountPath.get()) {
			return Optional.ofNullable(Strings.emptyToNull(customMountPath.get()));
		} else {
			return Optional.empty();
		}
	}

	public BooleanProperty usesReadOnlyMode() {
		return usesReadOnlyMode;
	}

	public StringProperty mountFlags() {
		return mountFlags;
	}

	public IntegerProperty maxCleartextFilenameLength() {
		return maxCleartextFilenameLength;
	}

	public ObjectProperty<WhenUnlocked> actionAfterUnlock() {
		return actionAfterUnlock;
	}

	public WhenUnlocked getActionAfterUnlock() {
		return actionAfterUnlock.get();
	}

	public BooleanProperty autoLockWhenIdle() {
		return autoLockWhenIdle;
	}

	public IntegerProperty autoLockIdleSeconds() {
		return autoLockIdleSeconds;
	}

	/* Hashcode/Equals */

	@Override
	public int hashCode() {
		return Objects.hash(id);
	}

	@Override
	public boolean equals(Object obj) {
		if (obj instanceof VaultSettings other && obj.getClass().equals(this.getClass())) {
			return Objects.equals(this.id, other.id);
		} else {
			return false;
		}
	}

}
