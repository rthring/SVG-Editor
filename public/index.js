// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {

      $.ajax({
      type: 'get',            //Request type
      dataType: 'json',       //Data type - we will use JSON for almost everything
      url: '/filenames',   //The server endpoint we are connecting to
      success: function (data) {
        var dropdown = document.getElementById('dropdownsvgs');
        var table = document.getElementById('fileLogTable');
        for (var i = 0; i < data.length; i++) {
          var option = document.createElement("option");
          option.text = data[i];
          option.value = data[i];
          dropdown.add(option);
          var row = table.insertRow(-1);
          var img = row.insertCell(0);
          img.innerHTML = "<a href="+data[i]+"><img src="+data[i]+" width=\"200\"></a>";
          var name = row.insertCell(1);
          name.innerHTML = "<a href="+data[i]+">"+data[i]+"</a>";
          var size = row.insertCell(2);
          size.innerHTML = "";
          $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/filesize',
            data: {
              value1: data[i]
            },
            success: function (data) {
              console.log("/filesize returned: ");
              console.log(data);
              var rows = document.getElementById('fileLogTable').rows;
              for (var i = 0; i < rows.length; i++) {
                if (rows[i].cells.length > 1) {
                  if (rows[i].cells[1].innerHTML == "<a href=\""+data.name+"\">"+data.name+"</a>") {
                    rows[i].deleteCell(2);
                    data.size = data.size / 1000;
                    data.size = Math.round(data.size);
                    rows[i].insertCell(2).innerHTML = data.size+"KB";
                  }
                }
              }
            },
            fail: function (error) {
              console.log("Error getting filesizes.");
              alert("There was an error gathering the filesizes")
            }
          });
          var numRects = row.insertCell(3);
          numRects.innerHTML = "";
          var numCircs = row.insertCell(4);
          numCircs.innerHTML = "";
          var numPaths = row.insertCell(5);
          numPaths.innerHTML = "";
          var numGroups = row.insertCell(6);
          numGroups.innerHTML = "";
          $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/numelements',
            data: {
              value1: data[i]
            },
            success: function (data) {
              console.log("/numelements returned data: ");
              console.log(data);
              var rows = document.getElementById('fileLogTable').rows;
              for (var i = 0; i < rows.length; i++) {
                if (rows[i].cells.length > 1) {
                  if (rows[i].cells[1].innerHTML == "<a href=\""+data.name+"\">"+data.name+"</a>") {
                    rows[i].deleteCell(3);
                    rows[i].insertCell(3).innerHTML = data.numRect;
                    rows[i].deleteCell(4);
                    rows[i].insertCell(4).innerHTML = data.numCirc;
                    rows[i].deleteCell(5);
                    rows[i].insertCell(5).innerHTML = data.numPaths;
                    rows[i].deleteCell(6);
                    rows[i].insertCell(6).innerHTML = data.numGroups;
                  }
                }
              }
            },
            fail: function (error) {
              console.log("Error getting number of elements for a file.");
              alert("There was an error trying to get the number of elements in the files");
            }
          });
        }
        if (data.length == 0) {
          table.insertRow(-1).innerHTML = "No files";
        }
      },
      fail: function(error) {
          console.log("Error getting filenames.");
          alert("There was an error getting the filenames");
      }
    });

    $('#createform').submit(function(e){
        e.preventDefault();
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/createsvg',
          data: {
            value1: document.getElementById('createfilename').value
          },
          success: function (data) {
            let table = document.getElementById('fileLogTable');
            let name = document.getElementById('createfilename').value;
            if (data.valid == 1) {
              if (table.rows[table.rows.length - 1].innerHTML == "No files") {
                table.deleteRow(-1);
              }
              let row = table.insertRow(-1);
              let cell = row.insertCell(0);
              cell.innerHTML = "<a href="+name+"><img src="+name+" width=\"200\"></a>";
              cell = row.insertCell(1);
              cell.innerHTML = "<a href="+name+">"+name+"</a>";
              cell = row.insertCell(2);
              cell.innerHTML = "0KB";
              cell = row.insertCell(3);
              cell.innerHTML = "1";
              cell = row.insertCell(4);
              cell.innerHTML = "0";
              cell = row.insertCell(5);
              cell.innerHTML = "0";
              cell = row.insertCell(6);
              cell.innerHTML = "0";
            } else {
              alert("SVG file was not created due to an invalid name.");
            }
          },
          fail: function (error) {
            alert("SVG file was not created due to an error.");
          }
        });
    });

    $('#fileselect').submit(function(e){
        document.getElementById('attributeview').innerHTML = "";
        document.getElementById('elementviewname').innerHTML = "";
        var cell = document.getElementById('imagedisplay');
        cell.innerHTML = "<a href="+$('#dropdownsvgs').val()+"><img src="+$('#dropdownsvgs').val()+" width=\"800\"></a>";
        document.getElementById('fileviewname').innerHTML = $('#dropdownsvgs').val();
        document.getElementById('edittitleanddesc').innerHTML = "";
        let form = document.createElement("FORM");
        form.setAttribute("id", "titleanddescform");
        form.setAttribute("ref", "titleanddescform");
        form.append("Fill out the form below to change the currently selected file's title and description")
        form.append(document.createElement("BR"));
        form.append("Enter new title below: ");
        form.append(document.createElement("BR"));
        let inputBox = document.createElement("INPUT");
        inputBox.setAttribute("type", "text");
        inputBox.setAttribute("value", "");
        inputBox.setAttribute("id", "titleinput");
        form.append(inputBox);
        form.append(document.createElement("BR"));
        form.append("Enter new description below: ");
        form.append(document.createElement("BR"));
        inputBox = document.createElement("INPUT");
        inputBox.setAttribute("type", "text");
        inputBox.setAttribute("value", "");
        inputBox.setAttribute("id", "descinput");
        form.append(inputBox);
        inputBox = document.createElement("INPUT");
        inputBox.setAttribute("type", "submit");
        inputBox.setAttribute("value", "Save changes");
        form.append(inputBox);
        document.getElementById('edittitleanddesc').append(form);
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/titleanddesc',
          data: {
            value1: document.getElementById('dropdownsvgs').value
          },
          success: function (data) {
            console.log(data);
            if (data.title == "") {
              document.getElementById('title').innerHTML = "Empty title";
            } else {
              document.getElementById('title').innerHTML = data.title;
            }
            if (data.desc == "") {
              document.getElementById('description').innerHTML = "Empty description";
            } else {
              document.getElementById('description').innerHTML = data.desc;
            }
          },
          fail: function (error) {
            document.getElementById('title').innerHTML = "Empty title";
            document.getElementById('description').innerHTML = "Empty description";
            console.log("Failed to get title and description for selected SVG.");
            alert("Failed to get the title and description for the selected SVG");
          }
        });
        $('#titleanddescform').submit(function(e){
          e.preventDefault();
          $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/changetitleanddesc',
            data: {
              value1: document.getElementById('dropdownsvgs').value,
              value2: document.getElementById('titleinput').value,
              value3: document.getElementById('descinput').value
            },
            success: function (data) {
              if (data.valid == 1) {
                $.ajax({
                  type: 'get',
                  dataType: 'json',
                  url: '/titleanddesc',
                  data: {
                    value1: document.getElementById('dropdownsvgs').value
                  },
                  success: function (data) {
                    console.log(data);
                    if (data.title == "") {
                      document.getElementById('title').innerHTML = "Empty title";
                    } else {
                      document.getElementById('title').innerHTML = data.title;
                    }
                    if (data.desc == "") {
                      document.getElementById('description').innerHTML = "Empty description";
                    } else {
                      document.getElementById('description').innerHTML = data.desc;
                    }
                  },
                  fail: function (error) {
                    document.getElementById('title').innerHTML = "Empty title";
                    document.getElementById('description').innerHTML = "Empty description";
                    alert("Failed to get title and description for selected SVG.");
                  }
                });
              } else {
                alert("The given title and description values were invalid so the SVG was not changed");
              }
            },
            fail: function (error) {
              alert("There was an error changing the SVG title");
            }
          });
        });
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/components',
          data: {
            value1: document.getElementById('dropdownsvgs').value
          },
          success: function (data) {
            console.log("/components returned data: ");
            console.log("data");
            let table = document.getElementById('svgcomponents');
            let old_rows = table.rows.length - 2;
            let dropdown = document.getElementById('dropdownelements');
            console.log("Deleting " + old_rows +" rows of the components table and options of the components dropdown.");
            if (old_rows > 0) {
              dropdown.remove(1);
            }
            while(old_rows > 0) {
              table.deleteRow(-1);
              dropdown.remove(1);
              old_rows--;
            }
            let svg = document.createElement("option");
            svg.text = "SVG";
            svg.value = "SVG";
            dropdown.add(svg);
            for (let i = 0; i < data[0].length; i++) {
              let row = table.insertRow(-1);
              row.insertCell(0).innerHTML = "Rectangle " + (i+1);
              row.insertCell(1).innerHTML = "Upper left corner: x = " + data[0][i].x + data[0][i].units + ", y = " + data[0][i].y + data[0][i].units + ", Width = " + data[0][i].w + data[0][i].units + ", Height = " + data[0][i].h + data[0][i].units;
              row.insertCell(2).innerHTML = data[0][i].numAttr;
              var option = document.createElement("option");
              option.text = "Rectangle " + (i+1);
              option.value = "Rectangle " + (i+1);
              dropdown.add(option);
            }
            for (let i = 0; i < data[1].length; i++) {
              let row = table.insertRow(-1);
              row.insertCell(0).innerHTML = "Circle " + (i+1);
              row.insertCell(1).innerHTML = "Centre: x = " + data[1][i].cx + data[1][i].units + ", y = " + data[1][i].cy + data[1][i].units + ", Radius = " + data[1][i].r;
              row.insertCell(2).innerHTML = data[1][i].numAttr;
              var option = document.createElement("option");
              option.text = "Circle " + (i+1);
              option.value = "Circle " + (i+1);
              dropdown.add(option);
            }
            for (let i = 0; i < data[2].length; i++) {
              let row = table.insertRow(-1);
              row.insertCell(0).innerHTML = "Path " + (i+1);
              row.insertCell(1).innerHTML = "Path data = " + data[2][i].d;
              row.insertCell(2).innerHTML = data[2][i].numAttr;
              var option = document.createElement("option");
              option.text = "Path " + (i+1);
              option.value = "Path " + (i+1);
              dropdown.add(option);
            }
            for (let i = 0; i < data[3].length; i++) {
              let row = table.insertRow(-1);
              row.insertCell(0).innerHTML = "Group " + (i+1);
              row.insertCell(1).innerHTML = data[3][i].children + " child elements";
              row.insertCell(2).innerHTML = data[3][i].numAttr;
              var option = document.createElement("option");
              option.text = "Group " + (i+1);
              option.value = "Group " + (i+1);
              dropdown.add(option);
            }
          },
          fail: function (error) {
            console.log("Failed to get components of SVG.");
            alert("An error occured while trying to get the components of the slected SVG");
          }
        });
    });
    $('#elementselect').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
          type: 'get',
          dataType: 'json',
          url: '/attributes',
          data: {
            value1: document.getElementById('fileviewname').innerHTML,
            value2: document.getElementById('dropdownelements').value
          },
          success: function (data) {
            console.log("/attributes returned data: ");
            console.log(data);
            document.getElementById('elementviewname').innerHTML = document.getElementById('dropdownelements').value;
            space = document.getElementById('attributeview');
            space.innerHTML = "";
            /*for (let property in data) {
              if (data.hasOwnProperty(property)) {
                space.append(property + ": " + data[property]);
                space.append(document.createElement("BR"));
              }
            }*/
            let form = document.createElement("FORM");
            form.setAttribute("id", "attributeform");
            form.setAttribute("ref", "attributeform");
            let i = 0;
            for (let property in data) {
              if (data.hasOwnProperty(property)) {
                form.append(property + ": ");
                form.append(document.createElement("BR"));
                let input = document.createElement("INPUT");
                input.setAttribute("type", "text");
                input.setAttribute("value", data[property]);
                input.setAttribute("id", "attribute" + i);
                form.append(input);
                form.append(document.createElement("BR"));
                i++;
              }
            }
            let input = document.createElement("INPUT");
            input.setAttribute("type", "submit");
            input.setAttribute("value", "Save changes");
            form.append(input);
            space.append(form);
            space.append(document.createElement("BR"));
            $('#attributeform').submit(function(e){
              e.preventDefault();
              console.log("Attempting to set attribute values")
              let i = 0;
              let attribute = document.getElementById("attribute"+i);
              console.log(1);
              while (document.getElementById("attribute"+i) != null) {
                let k = i;
                console.log(2);
                $.ajax({
                  type: 'get',
                  dataType: 'json',
                  url: '/setattribute',
                  data: {
                    value1: document.getElementById('fileviewname').innerHTML,
                    value2: document.getElementById('elementviewname').innerHTML,
                    value3: i,
                    value4: document.getElementById('attribute' + i++).value
                  },
                  success: function (data) {
                    if (data.valid == 1) {
                      console.log("Attribute successfully updated, refresh the page to see the updated SVG.");
                    } else {
                      console.log("Attribute was not updated.");
                      alert("A given attribute value was invalid so it was not saved to the SVG");
                    }
                  },
                  fail: function (error) {
                    console.log("Failed to save an update.");
                    alert("A given attribute value was invalid so it was not saved to the SVG");
                  }
                });
                while (k == i) {

                }
              }
            });

            form = document.createElement("FORM");
            form.setAttribute("id", "newattributeform");
            form.setAttribute("ref", "newattributeform");
            input = document.createElement("INPUT");
            form.append("You can create a new attribute below (attribute name | attribute value):");
            form.append(document.createElement("BR"));
            input.setAttribute("type", "text");
            input.setAttribute("value", "");
            input.setAttribute("id", "nameinput");
            form.append(input);
            input = document.createElement("INPUT");
            input.setAttribute("type", "text");
            input.setAttribute("value", "");
            input.setAttribute("id", "valueinput");
            form.append(input);
            input = document.createElement("INPUT");
            input.setAttribute("type", "submit");
            input.setAttribute("value", "Submit");
            form.append(input);
            space.append(form);
            $('#newattributeform').submit(function(e){
              e.preventDefault();
              console.log("Attempting to change attribute values");
              $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/newattribute',
                data: {
                  value1: document.getElementById('fileviewname').innerHTML,
                  value2: document.getElementById('elementviewname').innerHTML,
                  value3: document.getElementById('nameinput').value,
                  value4: document.getElementById('valueinput').value,
                },
                success: function (data) {
                  if (data.valid == 1) {
                    console.log("New attribute successfully added, refresh the page to see the updated SVG.");
                  } else {
                    console.log("New attribute was not successfully added.");
                    alert("The new attribute was invalid so it was not saved to the SVG");
                  }
                },
                fail: function (error) {
                  console.log("Failed to add the new attribute.");
                  alert("The new attribute was invalid so it was not saved to the SVG");
                }
              });
            });
          },
          fail: function (error) {
            alert("Failed to get attributes of element.");
          }
        });
    });
    document.getElementById('add').onclick = function(e) {
      e.preventDefault();
      document.getElementById('scaleoradd').innerHTML = "";
      let form = document.createElement("FORM");
      form.setAttribute("id", "addform");
      form.setAttribute("ref", "addform");
      if (document.getElementById('dropdownshapes').value == "Rectangle") {
        let input = document.createElement("INPUT");
        form.append("x: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
        input = document.createElement("INPUT");
        form.append("y: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
        input = document.createElement("INPUT");
        form.append("width: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
        input = document.createElement("INPUT");
        form.append("height: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
        input = document.createElement("INPUT");
        form.append("units: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
      } else {
        let input = document.createElement("INPUT");
        form.append("cx: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
        input = document.createElement("INPUT");
        form.append("cy: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
        input = document.createElement("INPUT");
        form.append("radius: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
        input = document.createElement("INPUT");
        form.append("units: ");
        form.append(document.createElement("BR"));
        input.setAttribute("type", "text");
        form.append(input);
        form.append(document.createElement("BR"));
      }
      let input = document.createElement("INPUT");
      input.setAttribute("type", "submit");
      input.setAttribute("id", "addbtn");
      form.append(input);
      document.getElementById('scaleoradd').append(form);
      $('#addform').submit(function(e){
        e.preventDefault();
        alert("Sorry this functionality has not been provided yet.");
      });
    };
    document.getElementById('scale').onclick = function(e) {
      e.preventDefault();
      document.getElementById('scaleoradd').innerHTML = "";
      let form = document.createElement("FORM");
      form.setAttribute("id", "scaleform");
      form.setAttribute("ref", "scaleform");
      let input = document.createElement("INPUT");
      form.append("scale: ");
      form.append(document.createElement("BR"));
      input.setAttribute("type", "text");
      form.append(input);
      form.append(document.createElement("BR"));
      input = document.createElement("INPUT");
      input.setAttribute("type", "submit");
      input.setAttribute("id", "scalebtn");
      form.append(input);
      document.getElementById('scaleoradd').append(form);
      $('#scaleform').submit(function(e){
        e.preventDefault();
        alert("Sorry this functionality has not been provided yet.");
      });
    };
});
