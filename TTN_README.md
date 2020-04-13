# Using the Particula TTN application

## Adding the particula application to (and setting up) your TTN account

- Make sure you have a TTN user account. If you don't have one yet, go to [The Things Network](https://www.thethingsnetwork.org/) and go to the 'Sign Up' page.
- Ask one of the application administrators to add you as a 'collaborator'. Make sure to provide him/here with your TTN username.
- Once they added you, go to the [TTN Console](https://console.thethingsnetwork.org/applications). You will find the 'particula' application there.

## Adding a new device

On the 'overview' tab scroll to the 'devices' section and click the 'register device' button. Now, what do you have to fill out here:

1. Device ID: Give the new device a descriptive name (if it's for development maybe add your name)
2. Device EUI: click the '🔀' button to generate a random value. 'this field will be generated' should appear in the box next to it.
3. App Key: Leave this as it is. 'this field will be generated' should also appear in the box.
4. App EUI: This is the key for the application itself (you cannot change this).

## Configuring the firmware

You now hold all the keys. You will need to use these to configure the 'Particula' firmware so your hardware can communicate with TTN.

## Checking the connection

After you added the 'particula' application to your account and you added your device. Then configured the firmware with the keys for your device. You can now check if the messages are being received by TTN by going to the 'data' tab.

By clicking on one of the received payloads you get an overview of the data (and metadata) thats available.
