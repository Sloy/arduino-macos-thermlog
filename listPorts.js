var Serialport = require('serialport');

// List serial ports:
Serialport.list()
    .then((ports) => {
        ports.forEach(function (port) {
            console.log(port.path);
        });
    })

function getConnectedArduino() {
    SerialPort.list()
        .then((ports) => {
            let matchingPort = ports.find(port => {
                manufacturer = port['manufacturer']
                return typeof manufacturer !== 'undefined' && manufacturer.includes('arduino')
            })
            return matchingPort.comName.toString();
        })
}
