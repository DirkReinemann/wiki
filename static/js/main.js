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
                        <button id='clear-filter' class='btn btn-default' type='button'>Clear</button>
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
            let result = getFilterButtonRow();
            result += "<br/>";
            result += getFileListRow(data);

            let content = $("#content");
            content.empty();
            content.append("<h1>FILES</h1><br/>");
            content.append(result);

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

            $("#clear-filter").click(function() {
                $("#filter").val("");
                clearFilterInput();
            });
        },
        error: function() {
            addErrorMessage("Error while loading the file list!");
        }
    });
}

function getSearchResultRow(filename, lines) {
    let template = `
        <div class='row'>
            <div class='col-md-12'>
                <div class='panel panel-default'>
                    <div class='panel-heading filename'>
                        <h3 class='panel-title'>{{filename}}</h3>
                    </div>
                    <div class='panel-body'>
                        <ul class='list-group'>{{lines}}</ul>
                    </div>
                </div>
            </div>
        </div>
    `;

    let items = "";
    lines.forEach(function(element) {
        items += "<li class='list-group-item'>" + element + "</li>";
    });
    return template.replace("{{filename}}", filename).replace("{{lines}}", items);
}

function loadSearchResults(keyword) {
    $.ajax({
        url: "search",
        method: "POST",
        data: { keyword: keyword },
        success: function(data) {
            let result = "";
            data.forEach(function(element) {
                result += getSearchResultRow(element.filename, element.lines);
            });
            let content = $("#content");
            content.empty();
            content.append("<h1>SEARCH RESULTS</h1><br/>");
            content.append(result);

            $(".filename").click(function() {
                let h3 = $(this).find("h3");
                loadFile(h3.text());
            });
        },
        error: function() {
            addErrorMessage("Error while searching for keyword '" + keyword + "'!");
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

    $("#clear-search").click(function() {
        $("#search").val("");
    });

    $("#search").keypress(function(event) {
        let search = $(this).val().trim();
        if (event.which === 13 && search.length > 0) {
            loadSearchResults(search);
        }
    });
});
