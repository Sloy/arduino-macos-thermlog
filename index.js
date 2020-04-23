const SerialPort = require("serialport");
const Readline = require('@serialport/parser-readline')
const child_process = require('child_process');
const osxTemp = require('osx-temperature-sensor');

const cpuLimitRegex = new RegExp("CPU_Speed_Limit\\s+=\\s(\\d+)");


async function getConnectedArduino() {
    const ports = await SerialPort.list();
    console.log("Available ports: ");
    console.log(ports);
    console.log();
    let matchingPort = ports.find(port => {
        manufacturer = port['manufacturer'];
        return typeof manufacturer !== 'undefined' && (manufacturer.includes('arduino') || manufacturer.includes('Arduino'));
    });
    return matchingPort.path.toString();
}

function cpuLimit() {
    let thermlog = child_process.execSync('pmset -g therm').toString()
    let match = thermlog.match(cpuLimitRegex)
    return match[1]
}

function cpuTemp() {
    return osxTemp.cpuTemperature().max
}

let lastCpuLimitValue = 0
function updateCpuLimitValue(port) {
    let newCpuLimitValue = cpuLimit()
    if (newCpuLimitValue != lastCpuLimitValue) {
        lastCpuLimitValue = newCpuLimitValue
        console.log(`Updating CPU Limit value: ${newCpuLimitValue}`)
        port.write("L"+newCpuLimitValue.toString()+",", function (error) {
            if (error) {
                return console.log('Error on write: ', err.message)
            }
        })
    }
}

let lastCpuTempValue = 0
function updateCpuTempValue(port){
    let newCpuTempValue = cpuTemp()
    if (newCpuTempValue != lastCpuTempValue) {
        lastCpuTempValue = newCpuTempValue
        console.log(`Updating CPU Temp value: ${newCpuTempValue}`)
        port.write("T"+newCpuTempValue.toString()+",", function (error) {
            if (error) {
                return console.log('Error on write: ', err.message)
            }
        })
    }
}

function loop(port) {
    setInterval(function () {
        updateCpuLimitValue(port)
        updateCpuTempValue(port)
    }, 2000)
}

getConnectedArduino()
    .then(portName => {
        console.log("Found arduino at " + portName)
        const port = new SerialPort(portName, {
            baudRate: 9600
        })
        // Print received data from arduino
        const lineParser = new Readline("\r\n")
        port.pipe(lineParser)
        lineParser.on("data", function (data) {
            console.log("> Received: " + data)
        })

        loop(port)
    })

