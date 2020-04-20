const SerialPort = require("serialport");
const Readline = require('@serialport/parser-readline')

const portName = "/dev/tty.usbmodem141101"

const port = new SerialPort(portName, {
    baudRate: 9600
});

const parser = new Readline("\r\n")
port.pipe(parser)

parser.on("data", function (data) {
    console.log("> Received: " + data)
})

function pingMessage() {
    let random = Math.trunc((Math.random() * 100))
    console.log(`Sending message ${random} ...`)
    port.write(random.toString(), function (error) {
        if (error) {
            return console.log('Error on write: ', err.message)
        }
        console.log('Message written')
    })
}

function loopPing() {
    setTimeout(function () {
        pingMessage()
        loopPing()
    }, 3000)
}

loopPing()