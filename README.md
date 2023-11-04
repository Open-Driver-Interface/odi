# OpenDriverInterface

![OpenDriverInterface Logo](https://avatars.githubusercontent.com/u/148554888?v=4)

OpenDriverInterface is a comprehensive framework designed for OS development hobbyists and professional kernel developers. It offers a seamless solution for integrating device drivers into your operating system through a well-documented interface and a cohesive abstraction layer. This framework is designed to be standalone, requiring minimal modifications for integration with your existing OS projects. It is versatile, catering to both those who wish to write their own device drivers and those looking to integrate a basic subset of device drivers into their OS.

## Features

- **Seamless Integration:** OpenDriverInterface provides a robust abstraction layer that simplifies the process of integrating device drivers into your operating system. This makes it a perfect choice for developers of all levels, from beginners to experts.

- **Well-Documented Interface:** We understand the importance of documentation, which is why we have provided extensive documentation for every aspect of the framework. This ensures that you have all the information you need at your fingertips to develop, modify, or troubleshoot your drivers.

- **Standalone Framework:** The framework is designed to be self-contained, meaning you can quickly integrate it into your current OS project with minimal changes. This saves time and effort while maintaining the integrity of your existing codebase.

- **Driver Implementations:** OpenDriverInterface currently includes implementations for the following device drivers:
    - AHCI (Advanced Host Controller Interface)
    - XHCI (eXtensible Host Controller Interface)
    - E1000 (Intel Gigabit Ethernet)
    - Serial Devices (UART, COM Port)

## Getting Started

To get started with OpenDriverInterface, follow these simple steps:

0. Install tree, make, gcc, nasm, qemu, fdisk, gdisk, and mtools.

1. Clone the OpenDriverInterface repository from [GitHub](https://github.com/Open-Driver-Interface/odi).

2. Check out the extensive documentation in the `docs` directory. It provides insights into the framework's architecture and usage.

3. Explore the provided driver implementations for inspiration or directly use them in your project.

4. Integrate the OpenDriverInterface framework into your OS project with minimal modifications, as guided by the documentation.

5. Start writing your device drivers or use the provided ones as a starting point.

## Contribution

We welcome contributions from the open-source community to enhance and expand OpenDriverInterface. If you want to contribute or report issues, please refer to our [Contribution Guidelines](https://github.com/Open-Driver-Interface/odi/blob/main/CONTRIBUTING.md) and [Code of Conduct](https://github.com/Open-Driver-Interface/odi/blob/main/CODE_OF_CONDUCT.md).

## License

OpenDriverInterface is released under the MIT License. For more details, refer to the [LICENSE](https://github.com/Open-Driver-Interface/odi/blob/main/LICENSE) file.

## Contact

If you have questions, suggestions, or need assistance, you can contact us at [contact@opendriverinterface.com](mailto:contact@opendriverinterface.com).

We hope that OpenDriverInterface simplifies your journey into OS development by providing a robust and well-documented framework for device driver integration. Happy coding! 🚀

## Acknowledgements

We would like to thank the following people for their contributions to OpenDriverInterface:

- Konect-V [github](https://github.com/konect-V)
- Marco Paland [email](mailto://info@paland.com)
- Absurd Poncho [github](https://github.com/Absurdponcho)
- The Limine project [github](https://github.com/limine-bootloader)