"use strict";

function addErrorMessage(message) {
    let content = $("#content");
    let temnplate = `
        <div class='col-md-6 col-md-offset-3'>
            <div class='alert alert-danger text-center'>{{message}}</div>
        </div>
    `;
    content.empty();
    content.append(temnplate.replace("{{message}}", message));
}

function getFilterButtonRow() {
    let template = `
        <div class="row">
            <div class="col-md-6">
                <div class='input-group'>
                    <span class='input-group-addon'>Filter</span>
                    <input id='filter' type='text' class='form-control'>
                    <span class='input-group-btn'>
                        <button id='clear' class='btn btn-default' type='button'>Clear</button>
                    </span>
                </div>
            </div>
        </div>
    `;
    return template;
}

function getFileListRow(filelist) {
    let template = `
        <div class="row">
            <div class="col-md-12">
                <ul id='filelist' class='filelist list-group'>{{items}}</ul>
            </div>
        </div>
    `;
    let items = "";
    filelist.sort().forEach(function(element) {
        items += "<li class='list-group-item'>" + element + "</li>";
    });
    return template.replace("{{items}}", items);
}

function loadFile(filename) {
    $.ajax({
        url: "file",
        method: "POST",
        data: { filename: filename },
        success: function(data) {
            let content = $("#content");
            content.empty();
            content.append(data);
        },
        error: function() {
            addErrorMessage("Error while loading the file '" + filename + "'!");
        }
    });
}

function clearFilterInput() {
    $("#filelist li").each(function() {
        $(this).show();
    });
}

function loadFilelist() {
    $.ajax({
        url: "filelist",
        success: function(data) {
            let content = getFilterButtonRow();
            content += "<br/>";
            content += getFileListRow(data);

            let c = $("#content");
            c.empty();
            c.append(content);

            $("#filelist li").click(function() {
                loadFile($(this).text());
            });

            $("#filter").keyup(function() {
                let value = $(this).val().trim();
                if (value.length > 0) {
                    $("#filelist li").each(function() {
                        let li = $(this);
                        if (li.text().toLowerCase().indexOf(value.toLowerCase()) !== -1) {
                            li.show();
                        }
                        else {
                            li.hide();
                        }
                    });
                } else {
                    clearFilterInput();
                }
            });

            $("#clear").click(function() {
                $("#filter").val("");
                clearFilterInput();
            });
        },
        error: function() {
            addErrorMessage("Error while loading the file list!");
        }
    });
}

$(document).ready(function () {
    loadFilelist();

    $("#pagetitle").click(function() {
        loadFilelist();
    });

    $("#link-filelist").click(function() {
        loadFilelist();
    });
});
