const SerialPort = require("serialport");
const Readline = require('@serialport/parser-readline')
const child_process = require('child_process');

const portName = "/dev/tty.usbmodem143101"
const cpuLimitRegex = new RegExp("CPU_Speed_Limit\\s+=\\s(\\d+)");

const port = new SerialPort(portName, {
    baudRate: 9600
});
/*
// Print received data from arduino
const lineParser = new Readline("\r\n")
port.pipe(lineParser)
lineParser.on("data", function (data) {
    console.log("> Received: " + data)
})
*/


function cpuLimit() {
    let thermlog = child_process.execSync('pmset -g therm').toString()
    let match = thermlog.match(cpuLimitRegex)
    return match[1]
}

let lastCpuLimitValue = 0
function updateCpuLimitValue() {
    let newCpuLimitValue = cpuLimit()
    if (newCpuLimitValue != lastCpuLimitValue) {
        lastCpuLimitValue = newCpuLimitValue
        console.log(`Updating CPU Limit value: ${newCpuLimitValue}`)
        port.write(newCpuLimitValue.toString(), function (error) {
            if (error) {
                return console.log('Error on write: ', err.message)
            }
        })
    }
}

function loop() {
    setTimeout(function () {
        updateCpuLimitValue()
        loop()
    }, 2000)
}
loop()
