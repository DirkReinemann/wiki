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
        </div><br/>
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

function searchElements(root, keyword, results) {
    let children = root.children();
    if (children.length > 0) {
        let content = root.text();
        children.each(function() {
            content = content.replace($(this).text(), "");
        });
        content = content.trim();
        if (content.length > 0 && content.toLowerCase().indexOf(keyword) > -1) {
            results.push(root);
        }

        children.each(function() {
            searchElements($(this), keyword, results);
        });
    }
    else {
        if (root.text().toLowerCase().indexOf(keyword) > -1) {
            results.push(root);
        }
    }
}

function clearFileFilterInput() {
    $("#file").find("*").each(function() {
        $(this).show();
    });
}

function loadFile(filename, keyword = "", index = 0) {
    $.ajax({
        url: "file",
        method: "POST",
        data: { filename: filename },
        success: function(data) {
            let content = $("#content");
            content.empty();
            content.append("<h1>WIKIPAGE</h1><br/>");
            content.append(getFilterButtonRow());
            content.append("<div class='row'><div class='col-md-12' id='file'>" + data + "</div></div>");

            if (keyword.length > 0 && index > 0) {
                let file = $("#file");
                let results = [];
                keyword = keyword.toLowerCase();
                searchElements(file, keyword, results);
                if (results.length >= index) {
                    let result = results[index -1];
                    $("html,body").animate({
                        scrollTop: result.offset().top - 10
                    });

                }

                results.forEach(function(result) {
                    let html = result.html();
                    let highlighted = "";
                    let pos = 0;
                    let oldpos = 0;
                    let len = keyword.length;
                    while ((pos = html.toLowerCase().indexOf(keyword, pos)) !== -1) {
                        let val = html.substring(pos, pos + len);
                        highlighted += html.substr(oldpos, pos);
                        highlighted += "<span class='highlight'>" + val + "</span>";
                        oldpos = pos;
                        pos++;
                    }
                    highlighted += html.substr(oldpos + len);
                    result.html(highlighted);
                });
            }

            $("#filter").keyup(function() {
                let value = $(this).val().trim().toLowerCase();
                if (value.length > 0) {
                    let file = $("#file");
                    let results = [];
                    searchElements(file, value, results);

                    file.find("*").each(function() {
                        $(this).hide();
                    });

                    results.forEach(function(element) {
                        element.show();
                        element.parents().show();
                    });
                }
                else {
                    clearFileFilterInput();
                }
            });

            $("#clear-filter").click(function() {
                $("#filter").val("");
                clearFileFilterInput();
            });
        },
        error: function() {
            addErrorMessage("Error while loading the file '" + filename + "'!");
        }
    });
}

function clearFileListFilterInput() {
    $("#filelist li").each(function() {
        $(this).show();
    });
}

function loadFilelist() {
    $.ajax({
        url: "filelist",
        success: function(data) {
            let result = getFilterButtonRow();
            result += getFileListRow(data);

            let content = $("#content");
            content.empty();
            content.append("<h1>FILES</h1><br/>");
            content.append(result);

            $("#filelist li").click(function() {
                loadFile($(this).text());
            });

            $("#filter").keyup(function() {
                let value = $(this).val().trim().toLowerCase();
                if (value.length > 0) {
                    $("#filelist li").each(function() {
                        let li = $(this);
                        if (li.text().toLowerCase().indexOf(value) !== -1) {
                            li.show();
                        }
                        else {
                            li.hide();
                        }
                    });
                } else {
                    clearFileListFilterInput();
                }
            });

            $("#clear-filter").click(function() {
                $("#filter").val("");
                clearFileListFilterInput();
            });
        },
        error: function() {
            addErrorMessage("Error while loading the file list!");
        }
    });
}

function formatNumber(number) {
    let result = "";
    if (number > 99) {
        result += number;
    }
    else if (number > 9) {
        result += "0" + number;
    }
    else {
        result += "00" + number;
    }
    result += ".";
    return result;
}

function getSearchResultRow(filename, lines, keyword) {
    let items = "";
    lines.forEach(function(element, index) {
        index++;
        items += "<li class='list-group-item fileresult' data-filename='" + filename;
        items += "' data-index='" + index;
        items += "' data-keyword='" + keyword + "'><span class='resultbadge'>";
        items += "<span class='badge'> " + formatNumber(index) + "</span></span>";
        items += "<span class='resulttext'>" + element + "</span></li>";
    });
    let result = `
        <div class='row'>
            <div class='col-md-12'>
                <div class='panel panel-default'>
                    <div class='panel-heading filename'>
    `;
    result += "<h3 class='panel-title'>" + filename + "</h3>";
    result += `
                    </div>
                    <div class='panel-body'>
    `;
    result += "<ul class='list-group'>" + items + "</ul>";
    result += `
                    </div>
                </div>
            </div>
        </div>
    `;
    return result;
}

function loadSearchResults(keyword) {
    $.ajax({
        url: "search",
        method: "POST",
        data: { keyword: keyword },
        success: function(data) {
            let result = "";
            data.forEach(function(element) {
                result += getSearchResultRow(element.filename, element.lines, keyword);
            });
            let content = $("#content");
            content.empty();
            content.append("<h1>SEARCH RESULTS</h1><br/>");
            content.append(result);

            if (data.length === 0) {
                content.append("<div class='row'><div class='col-md-12'>No search results.</div></div>");
            }

            $(".filename").click(function() {
                let h3 = $(this).find("h3");
                loadFile(h3.text());
            });

            $(".fileresult").click(function() {
                let li = $(this);
                let filename = li.attr("data-filename");
                let index = li.attr("data-index");
                let keyword = li.attr("data-keyword");
                loadFile(filename, keyword, index);
            });
        },
        error: function() {
            addErrorMessage("Error while searching for keyword '" + keyword + "'!");
        }
    });
}

function convert() {
    $.LoadingOverlay("show", {
        image: "static/img/loading.gif"
    });
    $.ajax({
        url: "convert",
        success: function() {
            $.LoadingOverlay("hide");
            loadFilelist();
        },
        error: function() {
            $.LoadingOverlay("hide");
            addErrorMessage("Error while converting markdown to html!");
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

    $("#convert").click(function() {
        convert();
    });

    $(window).scroll(function() {
        if ($(this).scrollTop() >= 50) {
            $("#back-to-top").show();
        } else {
            $("#back-to-top").hide();
        }
    });

    $("#back-to-top").click(function() {
        $("body,html").animate({
            scrollTop : 0
        });
    });
});
