


chrome.app.runtime.onLaunched.addListener(function() {
  // Center window on screen.
  var screenWidth = screen.availWidth;
  var screenHeight = screen.availHeight;
  var width = 850;
  var height = 550;

  chrome.app.window.create('index.html', {
    bounds: {
      width: width,
      height: height,
      left: 100,
      top: 50
    }
  });
});
