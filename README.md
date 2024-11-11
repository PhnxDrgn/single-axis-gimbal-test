# stm32-f3-blink
STM32 F3 NUCLEO board blink. This repo serves as a base project example/template for the NUCLEO-F303K8. The "blink" app is ran using a bare-metal implementation. STM32CubeMX has an option to add in FreeRTOS but that is not shown in this repo.

## Table of Contents
- [Requirements](#requirements)
- [Generateing Board Support Package Files](#generateing-board-support-package-files)
- [Setting Up Locally](#setting-up-locally)
- [Setting Up Docker](#setting-up-docker)
    - [Developing in Docker](#developing-in-docker)
- [Setting Up Functions in the BSP `main.c`](#setting-up-functions-in-the-bsp-mainc)
- [Building the Executable](#building-the-executable)
    - [Building Using VS Code CMake Extention](#building-using-vs-code-cmake-extention)
    - [Building Manually](#building-manually)

## Requirements:
- Notes:
    - If using Docker to build, there is no need to download the software in the Local Machine Build section but as of right now, debugging does not work with Docker.
    - If you are NOT using Docker to build and building on Windows, you can ignore the Docker Build section.

- STM32 File Generation
    - [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html): GUI for making board support packages (BSP)
    - [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html): Tool to read/write to STM32 memory. Used for flashing.
    - [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html): Eclipse based IDE for STM32. Used in this project for making debug config files.

- Local Machine Build (Windows)
    - [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm): Toolchain to compile for ARM embedded devices.
    - [openOCD](https://openocd.org/pages/getting-openocd.html): Used for attaching debugger. On Windows, binaries [here](https://github.com/openocd-org/openocd/releases/tag/latest)

- Docker Build:
    - [Docker Desktop](https://www.docker.com/products/docker-desktop/): Tool used to make containers for consistent development environment.

- Optional
    - [VS Code](https://code.visualstudio.com/): Open-source source code editor with tons of extensions.

- VS Code Extensions (Can be installed within VS Code)
    - [VS Code CMake Tools Extention](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools): A tool that lets you build/configure CMake projects without using the command line.
    - [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers): An extension that lets you work on the source code within the Docker container and use the terminal inside the container.

## Generateing Board Support Package Files
1. Open STM32BCubeMX
2. Load in `nucleo_f3.ioc` from `bsp/nucleo_f3`
    - This project has the following parameters set:
        - `PB3` set as a `GPIO_Output` and named `LED`
            - This pin is the LED pin on the NUCLEO-F303K8 board
        - The Toolchain/IDE option set to `Makefile`
        - Code Generator STM32Cube MCU packages and embedded software packs set to `Copy only the necessary library files`
            - This is to prevent it from including all the library files
3. Generate Code
    - This should generate files in the `bsp/nucleo_f3` directory

## Setting Up Locally
1. Download the software under the `Local Machine Build (Windows)` bullet in the [Requirements](#requirements) section.
2. Move the folders to somewhere that makes sense for you. I just put mine in my C:/ drive.
3. Add the path to the `bin` folders of the downloaded software to the Environment Variable path.
    1. Press Windows key.
    2. Type in `environment variables`
    3. Click on `Edit the system environment variables`
    4. Click on `Environment Variables...`
    5. Under `System variables`, edit `Path`.
    6. Press `new` and paste in the path to the bin of openOCD and arm toolchain files. The added paths should look something like:
    ```
    C:\arm-gnu-toolchain-13.2.Rel1-mingw-w64-i686-arm-none-eabi\bin
    C:\arm-gnu-toolchain-13.2.Rel1-mingw-w64-i686-arm-none-eabi\arm-none-eabi\bin
    C:\openocd\bin
    ```

## Setting Up Docker
1. Start Docker Desktop
2. Using the `docker_start.sh` script requires the ability to use bash. Linux systems can run it natively but Windows might require some setup. I personally use open a GitBash terminal in VS code to run it.

    Running the `docker_start.sh` script:
    ```
    ./docker_start.sh
    ```
    The script runs docker in detetched mode (-d) so if you want to attach to it, you can run `docker attach stm32-f3-blink`. The script also names the container the same name as the directory.

### Developing in Docker
After Docker is started, you can use the VS Code extension Dev Container and attach to a Docker container using the `Attach to Running Container...` command.
The directory that is set up as the volume and working directory for the project is under `/home/project`.

In the VS Code window that opened up for the container, navigate to that directory.

From here, make sure you have CMake Tools installed for VS Code because it would make it easier to configure/build the project. Details for building are below in [Building Using VS Code CMake Extention](#building-using-vs-code-cmake-extention)

## Setting Up Functions in the BSP `main.c`
1. The `app.c` file calls a function named `millis` and `toggleLed`. These functions are not yet set up.
2. Navigate to `main.h` located in `bsp/nucleo_f3/Core/Inc`.
3. Between the comment `USER CODE BEGIN EFP` add in prototypes for those functions. It should look something like
    ```
    /* USER CODE BEGIN EFP */
    uint32_t millis();
    void toggleLed();
    /* USER CODE END EFP */
    ```
4. Navigate to `main.c` located in `bsp/nucleo_f3/Core/Src`.
5. Add in the functions that were prototyped in the `main.h` file under the section `USER CODE BEGIN 0`. It should look like
    ```
    /* USER CODE BEGIN 0 */
    uint32_t millis()
    {
    return HAL_GetTick();
    }

    void toggleLed()
    {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
    /* USER CODE END 0 */
    ```
6. Include `app.h` in the `main.c` file under `USER CODE BEGIN Includes`. It should look like
    ```
    /* USER CODE BEGIN Includes */
    #include "app.h"
    /* USER CODE END Includes */
    ```
7. Finally, right above `USER CODE END WHILE` in the same `main.c` file. Add in the app function. It should look like
    ```
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        app();
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
    ```

## Building the Executable
### Building Using VS Code CMake Extention
1. Under `PROJECT OUTLINE` there should be the `blink` project name. Click on `Configure`.
2. Under the project there should be a `blink.elf (Executable)`. Click on `Build`
3. The `.elf`, `.hex`, and `.bin` files should be generated to the `build` directory.

### Building Manually
1. Configuring
    ```
    mkdir build
    cd build
    cmake ..
    ```
2. Building
    ```
    cmake --build .
    ```

## Debugger
