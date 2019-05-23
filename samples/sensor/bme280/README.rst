.. _bme280:

BME280 Humidity and Pressure Sensor
###################################

Overview
********

This sample application periodically reads temperature, pressure and humidity data from
the first available device that implements SENSOR_CHAN_AMBIENT_TEMP, SENSOR_CHAN_PRESS,
and SENSOR_CHAN_HUMIDITY. This sample checks the sensor in polling mode (without
interrupt trigger).

Building and Running
********************

This sample application uses an BME280 sensor connected to an Arduino 101 board via I2C.
Connect the sensor pins according to the connection diagram given in the `bme280 datasheet`_
at page 38.
There are two processor cores (x86 and ARC) on the Arduino 101. You will need to
flash both this sample's code on the ARC core (using the ``arduino101_sss`` board target),
and stub code on the x86 core (using the ``arduino_101`` board target), as shown below.


.. zephyr-app-commands::
   :zephyr-app: samples/sensors/bme280
   :board: arduino_101_sss
   :goals: flash
   :compact:

.. zephyr-app-commands::
   :zephyr-app: tests/booting/stub
   :board: arduino_101
   :goals: flash
   :compact:

Sample Output
=============
To check output of this sample , any serial console program can be used.
This example uses ``picocom`` on the serial port ``/dev/ttyUSB0``:

.. code-block:: console

        $ sudo picocom -D /dev/ttyUSB0

.. code-block:: console

        temp: 27.950000; press: 100.571027; humidity: 61.014648
        temp: 27.940000; press: 100.570269; humidity: 61.012695
        temp: 27.950000; press: 100.570695; humidity: 61.002929

.. _bme280 datasheet: https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf