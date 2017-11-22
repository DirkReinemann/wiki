$(document).ready(function () {
    loadFilelist();

    $("#title").click(function() {
        loadFilelist();
    });
});

function loadFilelist() {
    $.ajax({
        url: "filelist",
        success: function(data) {
            let content = "";
            content += "<ul id='filelist'>";
            data.sort().forEach(function(element) {
                content += "<li>" + element + "</li>";
            });
            content += "</ul>";

            let c = $("#content");
            c.empty();
            c.append(content);

            $("#filelist li").click(function() {
                loadFile($(this).text());
            });
        },
        error: function() {
            addErrorMessage("Error while loading the file list!");
        }
    });
}

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
