# ESP programming
- ставим ESP-IDF Installation Manager (без VSCode)
- Run 'source ~/.espressif/tools/activate_idf_v6.1.1.sh' to activate the IDF environment (python virtual environment)
- Run 'idf.py build' to build your project
- connect the device
- добавляем юзера в группу dialout для возможности flash  'sudo usermod -a -G dialout $USER'
- run 'idf.py flash'
- run 'idf.py monitor'  (можно сразу 'idf.py flash monitor')
- чтобы закрыть монитор: Ctrl+]


# GPIO

(GPIO & RTC GPIO - ESP32 - — ESP-IDF Programming Guide v5.3.2 documentation)[https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/gpio.html]

Strapping pin: GPIO0, GPIO2, GPIO5, GPIO12 (MTDI), and GPIO15 (MTDO) are strapping pins.

gpio_config for channels 36,39,34,35 returns error "GPIO can only be used as input mode",  from doc: "GPIO34-39 can only be set as input mode and do not have software-enabled pullup or pulldown functions"


# FreeRTOS

An event group is a set of binary flags (or bits), to each of which the application writer can assign a meaning. An RTOS task can enter the Blocked state to wait for one or more flags within the group to become active. The RTOS task does not consume any CPU time while it is in the Blocked state.

(RTOS task notifications - FreeRTOS)[https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/03-Direct-to-task-notifications/04-As-event-group]

(FreeRTOS (IDF) - ESP32 - — ESP-IDF Programming Guide v5.2.3 documentation)[https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/freertos_idf.html#smp-scheduler]

# TASKs
(FreeRTOS: мьютексы и критические секции)[https://habr.com/ru/articles/129445/]
