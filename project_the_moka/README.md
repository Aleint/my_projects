# Secure Mock Server with Dynamic Authentication

## Project Overview

This project was developed during my bachelor's degree internship and focused on the implementation of a robust security layer for a mock server environment. The primary goal was to move beyond a single, static access method and create a flexible system that supports multiple authentication schemes and dynamic access control through API keys.

The work encompassed backend development to handle complex authentication logic, DevOps tasks for containerization and SSL/TLS certificate management, and frontend development to create an intuitive user interface for managing security features. The entire system was built using a modern stack including Flask, Angular, Docker, and Firebase.

## Core Features

* **Multi-Scheme Authentication Backend**: The backend was engineered to validate incoming requests against endpoint-specific security schemes defined in a Swagger file. This allowed different endpoints to be protected by different methods. Supported schemes include:
    * **HTTP Basic Authentication**: Standard username/password authentication.
    * **Bearer Token Authentication**: Uses encrypted security tokens for access.
    * **Cookie-Based Authentication**: Manages sessions using HTTP cookies.

* **Dynamic API Key Management System**: A comprehensive system was developed to control access to the entire mock server instance.
    * **Generation**: Users can generate unique 16-character API keys, which modify the server's access URL and block the native link.
    * **Aliasing**: Custom aliases can be assigned to keys, making it easy to track who has access.
    * **Revocation**: Any API key can be revoked at any time, instantly disabling access for all holders of that key.
    * **Usage Tracking**: The system tracks the number of requests made and the status (enabled/disabled) of each API key.

* **Secure Containerization with Docker & SSL/TLS**: The application was packaged into a secure, portable Docker container.
    * A `Dockerfile` was created to ensure a consistent runtime environment.
    * A WSGI server was implemented for communication between the web server and the Python Flask application.
    * Nginx (`file.conf`) was configured to enforce HTTPS on port 443.
    * SSL/TLS certificates were managed using OpenSSL for testing, with the architecture designed for easy integration with Let's Encrypt for production environments.

* **Integrated Frontend Management UI**: A user-friendly dashboard was built using Angular for managing the API key system.
    * **View & Copy**: Users can view a list of all their created API keys and copy them to the clipboard with a single click.
    * **Create & Delete**: The UI provides intuitive buttons and icons (from Angular Material) for creating new keys, deleting individual keys, or deleting all keys to revert the server to open access.
    * **Confirmation Dialogs**: Custom components were created to display confirmation dialogs for critical actions like key deletion, preventing accidental changes.

## Technology Stack

* **Backend**: Python, Flask
* **Frontend**: Angular, TypeScript, Angular Material, CSS, Observables (for state management)
* **Database**: Firebase Realtime Database (NoSQL)
* **DevOps & Security**: Docker, Nginx, OpenSSL, Let's Encrypt
* **API Specification**: Swagger (OpenAPI)

## ⚠️ Disclaimer
Please note: The source code for this project is proprietary and cannot be shared publicly. This README serves as a technical overview of the project's architecture and my contributions.
