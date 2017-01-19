'use strict';

// Copyright 2017 Chris Williams <lizhenzhou1026@gmail.com>

const opcda = require('./build/Release/opcda');

opcda.OnDataChange(function (err, data) {
    if(err)console.log(err);
    console.log(data);
})
