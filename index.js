'use strict';

// Copyright 2017 Chris Williams <lizhenzhou1026@gmail.com>

const opcda = require('./build/Release/opcda');

opcda.Init("DESKTOP-JFJ715E", {
    ProgId: "Kepware.KEPServerEX.V6",
    itemNames: [
        "Data Type Examples.16 Bit Device.R Registers.Short1",
        "Data Type Examples.16 Bit Device.R Registers.Short2"
    ],
    OnDataChange: function (err, data) {
        if (err) console.log(err);
        console.log(data);
    }
}, function (err) {
    if (err) console.log(err);
})

//opcda.Init(function (err, data) {
//    if(err)console.log(err);
//    console.log(data);
//})
