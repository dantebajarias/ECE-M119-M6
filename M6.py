import asyncio
import struct
import time
from bleak import BleakScanner, BleakClient

DEVICE_NAME = 'Nano 33 IoT'
GESTURE_UUID = '00002101-0000-1000-8000-00805f9b34fb'
SCAN_TIME = 5

async def main():
    address = None
    #Scan for BLE device
    while True:
        print('Scanning for device')
        devices = await BleakScanner.discover()
        #List out all discoverable devices
        for d in devices:
            print(f'{d.name}')
            #If target device found, connect
            if d.name == DEVICE_NAME:
                address = str(d.address)
                print(f'Found {DEVICE_NAME}! Address: {address}')
                break
        if address is not None:
            break
        time.sleep(SCAN_TIME)
    async with BleakClient(address) as client:
        print(f'Connected to {address}')
        prev_gesture = -1
        while client.is_connected:
            gesture_bytes = await client.read_gatt_char(GESTURE_UUID)
            gesture = struct.unpack('<i', gesture_bytes)[0]
            if gesture != prev_gesture:
                print(gesture)
                prev_gesture = gesture
        print(f'Disconnected from {address}')


if __name__ == '__main__':
    asyncio.run(main())

