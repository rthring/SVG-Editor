'use strict'

// C library API
const ffi = require('ffi-napi');
var svgParser = ffi.Library("./libsvgparse", {
  "getTitleAndDescFromName": ["string", ["string"]],
  "svgJSONFromName": ["string", ["string"]],
  "componentsJSON": ["string", ["string"]],
  "checkValid": ["int", ["string"]],
  "attributesSVG": ["string", ["string"]],
  "attributesRectangle": ["string", ["string", "int"]],
  "attributesCircle": ["string", ["string", "int"]],
  "attributesPath": ["string", ["string", "int"]],
  "attributesGroup": ["string", ["string", "int"]],
  "setAttributeSVG": ["int", ["string", "int", "string"]],
  "setAttributeRectangle": ["int", ["string", "int", "int", "string"]],
  "setAttributeCircle": ["int", ["string", "int", "int", "string"]],
  "setAttributePath": ["int", ["string", "int", "int", "string"]],
  "setAttributeGroup": ["int", ["string", "int", "int", "string"]],
  "newAttributeSVG": ["int", ["string", "string", "string"]],
  "newAttributeRectangle": ["int", ["string", "int", "string", "string"]],
  "newAttributeCircle": ["int", ["string", "int", "string", "string"]],
  "newAttributePath": ["int", ["string", "int", "string", "string"]],
  "newAttributeGroup": ["int", ["string", "int", "string", "string"]],
  "changeTitleAndDesc": ["int", ["string", "string", "string"]],
  "createSVG": ["int", ["string"]]
})

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
/*app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;
  // Use the mv() method to place the file somewhere on your server

  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }
    let valid = svgParser.checkValid('uploads/' + uploadFile.name);
    if (valid == 1) {
      res.redirect('/');
    } else {
      fs.unlinkSync('uploads/' + uploadFile.name);
      console.log("Invalid file uploaded. Did not upload file.")
      return res.status(400).send('Invalid file. Did not upload.');
    }
  });
});*/

app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
  let uploadFile = req.files.uploadFile;
  fs.readdir(__dirname+'/uploads/', function(err, items) {
    for (let i = 0; i < items.length; i++) {
      if (uploadFile.name == items[i]) {
        return res.status(400).send('Upload denied. File had the same name as an uploaded file.')
      }
    }
    // Use the mv() method to place the file somewhere on your server
    uploadFile.mv('uploads/' + uploadFile.name, function(err) {
      if(err) {
        return res.status(500).send(err);
      }
      let valid = svgParser.checkValid('uploads/' + uploadFile.name);
      if (valid == 1) {
        res.redirect('/');
      } else {
        fs.unlinkSync('uploads/' + uploadFile.name);
        console.log("Invalid file uploaded. Did not upload file.")
        return res.status(400).send('Invalid file. Did not upload.');
      }
    });
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************

app.get('/filenames', function(req , res) {
  fs.readdir(__dirname+'/uploads/', function(err, items) {
    res.send(items);
  });
});

app.get('/filesize', function(req , res) {
  fs.stat(__dirname+'/uploads/'+req.query.value1, function(err, stats) {
    res.send({
      size: stats["size"],
      name: req.query.value1
    });
  });
});

app.get('/titleanddesc', function(req , res) {
  let retstr = svgParser.getTitleAndDescFromName("uploads/"+req.query.value1);
  retstr = retstr.replace("\n", "<br>");
  res.send(retstr);
});

app.get('/numelements', function(req , res) {
  let nums = JSON.parse(svgParser.svgJSONFromName("uploads/"+req.query.value1));
  console.log("Sending the following as a JSON");
  nums.name = req.query.value1;
  console.log(nums);
  res.send(nums);
});

app.get('/components', function(req , res) {
  let retstr = svgParser.componentsJSON("uploads/"+req.query.value1);
  console.log("Sending "+retstr+" as a JSON");
  res.send(retstr);
});

app.get('/attributes', function(req , res) {
  let element = req.query.value2;
  if (element.includes("SVG")) {
    console.log("Finding components of " + element);
    res.send(svgParser.attributesSVG("uploads/"+req.query.value1));
  } else if (element.includes("Rectangle") == true) {
    console.log("Finding components of " + element);
    res.send(svgParser.attributesRectangle("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1)));
  } else if (element.includes("Circle") == true) {
    console.log("Finding components of " + element);
    res.send(svgParser.attributesCircle("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1)));
  } else if (element.includes("Path") == true) {
    console.log("Finding components of " + element);
    res.send(svgParser.attributesPath("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1)));
  } else if (element.includes("Group") == true) {
    console.log("Finding components of " + element);
    res.send(svgParser.attributesGroup("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1)));
  } else {
    res.send("{}");
  }
});

app.get('/setattribute', function(req , res) {
  let element = req.query.value2;
  console.log("In /setattribute, arguments are "+req.query);
  if (element.includes("SVG") == true) {
    let i = svgParser.setAttributeSVG("uploads/"+req.query.value1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Rectangle") == true) {
    let i = svgParser.setAttributeRectangle("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Circle") == true) {
    let i = svgParser.setAttributeCircle("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Path") == true) {
    let i = svgParser.setAttributePath("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Group") == true) {
    let i = svgParser.setAttributeGroup("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else {
    res.send("{\"valid\":0}");
  }
});

app.get('/newattribute', function(req , res) {
  let element = req.query.value2;
  console.log("In /newattribute, arguments are: "+req.query);
  if (element.includes("SVG") == true) {
    let i = svgParser.newAttributeSVG("uploads/"+req.query.value1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Rectangle") == true) {
    let i = svgParser.newAttributeRectangle("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Circle") == true) {
    let i = svgParser.newAttributeCircle("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Path") == true) {
    let i = svgParser.newAttributePath("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else if (element.includes("Group") == true) {
    let i = svgParser.newAttributeGroup("uploads/"+req.query.value1, req.query.value2.charAt(req.query.value2.length - 1) - 1, req.query.value3, req.query.value4);
    res.send("{\"valid\":"+i+"}");
  } else {
    res.send("{\"valid\":0}");
  }
})

app.get('/changetitleanddesc', function(req , res) {
  console.log("In /changetitleanddesc, arguments are: "+req.query);
  let i = svgParser.changeTitleAndDesc("uploads/"+req.query.value1, req.query.value2, req.query.value3);
  res.send("{\"valid\":"+i+"}");
});

app.get('/createsvg', function(req , res) {
  let name = req.query.value1 + ".svg";
  console.log("Trying to save SVG with name: "+name);
  if (name.includes(" ") == true) {
    res.send("{\"valid\":0}");
    console.log("Saving failed because name had spaces");
  }
  fs.readdir(__dirname+'/uploads/', function(err, items) {
    for (let i = 0; i < items.length; i++) {
      if (items[i] == name) {
        console.log("Saving failed because file had same name as other file")
        res.send("{\"valid\":0}");
      }
    }
    let i = svgParser.createSVG("uploads/"+name);
    res.send("{\"valid\":"+i+"}");
  });

})

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
