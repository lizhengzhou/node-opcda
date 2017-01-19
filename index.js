'use strict';

// Copyright 2017 Chris Williams <lizhenzhou1026@gmail.com>

const opcda = require('./build/Release/opcda');
//var stream = require('stream');
//var util = require('util');
//opcda.list("DESKTOP-JFJ715E", function (err, progs) {
//    if (err)throw err;
//    progs.forEach(function(prog) {
//        console.log(prog)
//    });
//})
//stream.Stream.call(this)


//util.inherits(opcda, stream.Stream);

opcda.hello(function(data) {
    console.log(data);
})
