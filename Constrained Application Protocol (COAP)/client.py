# coap_client.py
# Python 3.x required
# Install dependency: pip install aiocoap

import asyncio
from aiocoap import *

async def main():
    # ESP32 IP address change করুন
    esp32_ip = "10.15.18.13"  # <-- এখানে আপনার ESP32 WiFi IP দিন
    uri = f"coap://{esp32_ip}/hello"

    protocol = await Context.create_client_context()

    request = Message(code=GET, uri=uri)

    try:
        response = await protocol.request(request).response
    except Exception as e:
        print(f"Failed to fetch resource: {e}")
    else:
        print(f"Response Code: {response.code}")
        print(f"Payload: {response.payload.decode('utf-8')}")

if __name__ == "__main__":
    asyncio.run(main())
