'use strict';

// Copyright 2017 lizz <lizhenzhou1026@gmail.com>

const opcda = require('./build/Release/opcda');

opcda.Init("DESKTOP-JFJ715E", {
    ProgId: "Kepware.KEPServerEX.V6",
    Rate: 1000,
    DeadBand:0.0,
    ItemNames: [
        "_System._ProjectTitle",
        "_System._DateTime",
        "Data Type Examples.16 Bit Device.R Registers.Boolean1",
        //"Data Type Examples.16 Bit Device.R Registers.Double1",        
        //"Data Type Examples.16 Bit Device.R Registers.DWord1",        
        //"Data Type Examples.16 Bit Device.R Registers.Float1",        
        //"Data Type Examples.16 Bit Device.R Registers.Long1",        
        //"Data Type Examples.16 Bit Device.R Registers.Short1",        
        //"Data Type Examples.16 Bit Device.R Registers.Word1",        
    ],
    OnDataChange: function (err, data) {
        if (err) console.log(err);
        console.log(data);
    }
}, function (err) {
    if (err) console.log(err);
})
console.log('good');

