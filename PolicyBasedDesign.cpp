// Example usage of policy based design pattern for communication policies in device drivers
// Date:        November 11, 2021
// Author:      Caglayan DOKME, caglayandokme@gmail.com

/** Libraries **/
#include <iostream>
#include <stdint.h>
#include <utility>

// Base abstract class for communication
class CommDevice {
public:
	CommDevice() 			{ std::cout << "Communication device created!" << std::endl; }
	virtual ~CommDevice() 	{ std::cout << "Communication device destroyed!" << std::endl; }

	virtual void send() = 0;
	virtual void receive() = 0;
};

// Policy of UART
class Uart : public CommDevice {
public:
	Uart() 	{ std::cout << "UART initialized." 	<< std::endl; }
	~Uart() { std::cout << "UART destroyed!" 	<< std::endl; }

	void send() 	final { std::cout << "Sending over UART."   << std::endl; }
	void receive() 	final { std::cout << "Receiving over UART." << std::endl; }
};

// Policy of SPI
class Spi : public CommDevice {
public:
	Spi() 	{ std::cout << "SPI initialized." 	<< std::endl; }
	~Spi() 	{ std::cout << "SPI destroyed!" 	<< std::endl; }

	void send() 	final { std::cout << "Sending over SPI." 	<< std::endl; }
	void receive() 	final { std::cout << "Receiving over SPI." 	<< std::endl; }
};

// External device driver class with template communication policy
template<class CommPolicy>
class ExternalDevice {
public:
    // Constructor
	ExternalDevice(std::string&& deviceName) : name(std::move(deviceName))
	{
		std::cout << name << ": Device driver created." << std::endl;
		std::cout << name << ": Applying the communication policy." 	<< std::endl;

		commDevice = new CommPolicy;

		if(nullptr == commDevice)
			std::cout << name << ": Communication policy failed." << std::endl;
	}

    // Destructor
	~ExternalDevice()
	{
		delete commDevice;

		std::cout << name << ": Device driver destroyed." << std::endl;
	}

    /** Member methods **/
	void sendData()
	{
		std::cout << name << ": Sending data.." << std::endl;
		commDevice->send();
	}

	void receiveData()
	{
		std::cout << name << ": Receiving data.." << std::endl;
		commDevice->receive();
	}

private:
    /** Members **/
	std::string name;
	CommPolicy* commDevice{nullptr};
};

int main() {
    // Creation of device drivers
	ExternalDevice<Spi>     device1("Device 1");
	ExternalDevice<Uart>    device2("Device 2");

	std::cout << std::endl;

    // Sending data to devices
	device1.sendData();
	device2.sendData();

	std::cout << std::endl;

    // Receiving data from devices
	device1.receiveData();
	device2.receiveData();

	std::cout << std::endl;

	std::cout << "Program finished." << std::endl << std::endl;

	return 0;
}
