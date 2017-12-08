"use strict";

let loadingImage = "data:image/gif;base64,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";

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
                searchElements(file, keyword.toLowerCase(), results);
                if (results.length >= index) {
                    $("html,body").animate({
                        scrollTop: results[index - 1].offset().top - 10
                    });
                }
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
        image: loadingImage
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
