# Contribution to the "Edge Infrastructure Ansible" Project

This document outlines my contributions to the open-source project **Edge Infrastructure Ansible**, developed in collaboration with the NetGroup team at Politecnico di Torino.

The main project consists of a set of Ansible playbooks designed to automate the deployment of a complete Edge infrastructure based on Kubernetes and other cloud-native technologies.

**Official Project Repository:** [https://github.com/netgroup-polito/edge-infrastructure-ansible](https://github.com/netgroup-polito/edge-infrastructure-ansible)


## My Contributions

My work focused on extending the project's capabilities to enhance its flexibility and keep it up-to-date. My primary responsibilities and implementations included:

### 1. KubeVirt Integration
I extended the Ansible playbooks to include the optional installation of **KubeVirt**. This feature enables the management of virtual machines (VMs) directly within a Kubernetes cluster, alongside containers.
* **Optional Installation**: Users can choose to install KubeVirt either through a configuration file or by responding to a prompt during the playbook's execution.
* **KubeVirt-Manager Integration**: Alongside KubeVirt, I integrated the installation of `kubevirt-manager`, a graphical user interface that significantly simplifies VM management within the cluster.

### 2. Liqo Version Update
I managed the version update for **Liqo**, a key tool for creating a multi-cluster continuum, upgrading it to version **v1.0.0**. This ensured compatibility with the latest features and security patches offered by the Liqo project.

### 3. Initial Dashboard Update
I updated the project's initial dashboard to reflect the newly added functionalities and improve the overall user experience. This made it easier for end-users to get a comprehensive overview of the newly deployed infrastructure, including the new components.
