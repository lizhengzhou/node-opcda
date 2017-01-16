'use strict';

// Copyright 2017 Chris Williams <lizhenzhou1026@gmail.com>

const opcda = require('./build/Release/opcda');

opcda.list("DESKTOP-JFJ715E", function (err, progs) {
    if (err)throw err;
    progs.forEach(function(prog) {
        console.log(prog)
    });
})
