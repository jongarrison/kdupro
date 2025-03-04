#include "pages.h"

namespace pages {
    const char* getIndexPage() {
        // Placeholder implementation
return R"---(
<html>
<head>
    <title>KduPro</title>
    <script type="text/javascript">
        function updateValues() {
            fetch('/state', {method: 'GET'})
                .then(response => response.json())
                .then(data => {
                    document.getElementById("light").innerHTML = data.light ? "Yes" : "No";
                    document.getElementById("batt").innerHTML = data.batt;
                });
        }
        document.addEventListener("DOMContentLoaded", updateValues);
    </script>
</head>
<body>
<h1>KduPro</h1>

<div>
    <h2>Values</h2>
    <p>Is light on: <span id="light">loading...</span></p>
    <p>Battery: <span id="batt">loading...</span></p>
</div>

<div>
    <h2>Actions</h2>
    <button onclick="fetch('/lighton', {method: 'GET'}).then(updateValues)">Turn light on</button>
    <button onclick="fetch('/lightoff', {method: 'GET'}).then(updateValues)">Turn light off</button>
</div>


</body>
</html>
)---";
    }
}