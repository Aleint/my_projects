import org.gradle.internal.os.OperatingSystem

plugins {
    `java-library`
    `java`
    `maven-publish`
    id("org.openjfx.javafxplugin") version "0.0.10"
//    application
}

javafx {
    version = "18"
    modules("javafx.base", "javafx.graphics", "javafx.controls", "javafx.fxml")
}

repositories {
    mavenLocal()
    mavenCentral()
}

dependencies {
    api("org.cryptomator:cryptofs:2.4.0") {
        exclude(group = "com.github.jnr")
    }
    api("org.cryptomator:fuse-nio-adapter:1.3.3") {
        exclude(group = "com.github.jnr")
    }
    api("org.cryptomator:dokany-nio-adapter:1.3.3") {
        exclude(group = "com.github.jnr")
    }
    api("org.cryptomator:webdav-nio-adapter:1.2.7") {
        exclude(group = "com.github.jnr")
    }
    api("org.cryptomator:integrations-api:1.1.0-beta1") {
        exclude(group = "com.github.jnr")
    }
    api("org.slf4j:slf4j-api:2.0.13")
    api("ch.qos.logback:logback-core:1.4.6")
    api("ch.qos.logback:logback-classic:1.4.6")
    api("org.apache.commons:commons-lang3:3.12.0")
    api("com.auth0:java-jwt:3.19.1")
    api("com.tobiasdiez:easybind:2.2")
    api("com.nulab-inc:zxcvbn:1.5.2")
    api("com.google.guava:guava:31.1-jre")
    api("com.google.dagger:dagger:2.41")
    api("com.google.code.gson:gson:2.9.0")

    runtimeOnly("com.github.jnr:jnr-ffi:2.2.10")
    runtimeOnly("com.github.jnr:jnr-posix:3.1.15")

    annotationProcessor("com.google.dagger:dagger-compiler:2.41") // Use annotationProcessor for Java

    testImplementation("org.junit.jupiter:junit-jupiter:5.8.1")
    testImplementation("org.mockito:mockito-core:4.4.0")
    testImplementation("org.hamcrest:hamcrest:2.2")
    testImplementation("org.openjfx:javafx-swing:18")
    testImplementation("com.google.jimfs:jimfs:1.2")
}

group = "org.cryptomator"
version = "1.7.0b-SECube Integration 1.0"
description = "Cryptomator Desktop App w/ SECube"

java {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
}



publishing {
    publications.create<MavenPublication>("maven") {
        from(components["java"])
    }
}


tasks.register<Exec>("generateNativeHeader") {
    description = "Generate JNI header file from Java class"
    group = "build"
    val os = OperatingSystem.current()

    commandLine = if (os.isWindows) {
        listOf(
            "javac", "-h", "src\\main\\cpp",
            "src\\main\\java\\org\\cryptomator\\secube\\SECubeConnector.java",
            "src\\main\\java\\org\\cryptomator\\secube\\exceptions\\SECubeException.java",
            "src\\main\\java\\org\\cryptomator\\secube\\exceptions\\SECubeLoginException.java",
            "src\\main\\java\\org\\cryptomator\\secube\\exceptions\\SECubeDatabaseException.java"
        )
    } else {
        listOf(
            "javac", "-h", "src/main/cpp",
            "src/main/java/org/cryptomator/secube/SECubeConnector.java",
            "src/main/java/org/cryptomator/secube/exceptions/SECubeException.java",
            "src/main/java/org/cryptomator/secube/exceptions/SECubeLoginException.java",
            "src/main/java/org/cryptomator/secube/exceptions/SECubeDatabaseException.java"
        )
    }

    doLast {
        exec {
            workingDir =
                file(if (os.isWindows) "src\\main\\java\\org\\cryptomator\\secube" else "src/main/java/org/cryptomator/secube")
            commandLine = if (os.isWindows) {
                listOf(
                    "cmd", "/c", "del",
                    "SECubeConnector.class",
                    "exceptions\\SECubeException.class",
                    "exceptions\\SECubeLoginException.class",
                    "exceptions\\SECubeDatabaseException.class"
                )
            } else {
                listOf(
                    "rm",
                    "SECubeConnector.class",
                    "exceptions/SECubeException.class",
                    "exceptions/SECubeLoginException.class",
                    "exceptions/SECubeDatabaseException.class"
                )
            }
        }
    }
}


tasks.register<Exec>("buildNativeLib") {
    description = "Build native library using CMake"
    group = "build"
    dependsOn("generateNativeHeader")

    val os = OperatingSystem.current()

    workingDir =
        file(if (os.isWindows) "src\\main\\cpp\\" else "src/main/cpp")

    if (os.isWindows) {
        // Windows
        commandLine("cmd", "/c", "cmake", "-S", ".", "-B", "build")
    } else {
        // Linux e macOS
        commandLine("cmake", "-S", ".", "-B", "build")
    }

    doLast {
        exec {
            workingDir =
                file(if (os.isWindows) "src\\main\\cpp\\" else "src/main/cpp")

            if (os.isWindows) {
                // Windows
                commandLine("cmd", "/c", "cmake", "--build", "build")
            } else {
                // Linux e macOS
                commandLine("cmake", "--build", "build")
            }
        }

    }
}

tasks.withType<JavaCompile> {
    dependsOn("buildNativeLib")
    options.encoding = "UTF-8"
}

tasks.named<JavaCompile>("compileJava") {
    dependsOn("generateNativeHeader")
}

tasks.withType<Javadoc> {
    options.encoding = "UTF-8"
}

tasks.register("cleanNativeLib") {
    doLast {
        delete(fileTree("src/main/cpp/build") {
            include("**/*")
        })
    }
}

tasks.register<JavaExec>("run") {
    group = "application"
    description = "Runs my application"
    mainClass.set("org.cryptomator.launcher.Cryptomator")

    val os = OperatingSystem.current()
    val homeDir = System.getProperty("user.home")

// Usa File per creare percorsi che funzionano correttamente su Windows e Linux/macOS
    val settingsPath = if (os.isWindows) File(homeDir, "AppData\\Roaming\\Cryptomator\\settings.json").absolutePath
    else File(homeDir, ".config/Cryptomator/settings.json").absolutePath

    val ipcSocketPath = if (os.isWindows) File(homeDir, "AppData\\Roaming\\Cryptomator\\ipc.socket").absolutePath
    else File(homeDir, ".config/Cryptomator/ipc.socket").absolutePath

    val logDir = if (os.isWindows) File(homeDir, "AppData\\Roaming\\Cryptomator\\logs").absolutePath
    else File(homeDir, ".local/share/Cryptomator/logs").absolutePath

    val pluginDir = if (os.isWindows) File(homeDir, "AppData\\Roaming\\Cryptomator\\plugins").absolutePath
    else File(homeDir, ".local/share/Cryptomator/plugins").absolutePath

    val mountPointsDir = if (os.isWindows) File(homeDir, "AppData\\Roaming\\Cryptomator\\mnt").absolutePath
    else File(homeDir, ".local/share/Cryptomator/mnt").absolutePath

    jvmArgs = listOf(
        "-Djdk.gtk.version=2",
        "-Duser.language=en",
        "-Dcryptomator.settingsPath=$settingsPath",
        "-Dcryptomator.ipcSocketPath=$ipcSocketPath",
        "-Dcryptomator.logDir=$logDir",
        "-Dcryptomator.pluginDir=$pluginDir",
        "-Dcryptomator.mountPointsDir=$mountPointsDir",
        "-Dcryptomator.showTrayIcon=true",
        "-Xss20m",
        "-Xmx512m"
    )



    classpath = sourceSets["main"].runtimeClasspath

    if (os.isWindows) {
        val debugDll = file("src/main/cpp/build/Debug/SECubeConnector.dll")

        if (debugDll.exists()) {
            systemProperty("java.library.path", "src/main/cpp/build/Debug/")
        } else {
            systemProperty("java.library.path", "src/main/cpp/build/Release/")
        }
    }
    else {
        systemProperty("java.library.path", "src/main/cpp/build")
    }
}
