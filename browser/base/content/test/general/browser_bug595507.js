/**
 * Make sure that the form validation error message shows even if the form is in an iframe.
 */
add_task(async function() {
  let uri = "<iframe src=\"data:text/html,<iframe name='t'></iframe><form target='t' action='data:text/html,'><input required id='i'><input id='s' type='submit'></form>\"</iframe>";

  var gInvalidFormPopup = document.getElementById("invalid-form-popup");
  ok(gInvalidFormPopup,
     "The browser should have a popup to show when a form is invalid");

  let tab = gBrowser.addTab();
  let browser = gBrowser.getBrowserForTab(tab);
  gBrowser.selectedTab = tab;

  await promiseTabLoadEvent(tab, "data:text/html," + escape(uri));

  let popupShownPromise = promiseWaitForEvent(gInvalidFormPopup, "popupshown");

  await ContentTask.spawn(browser, {}, async function() {
    content.document.getElementsByTagName("iframe")[0]
           .contentDocument.getElementById("s").click();
  });
  await popupShownPromise;

  await ContentTask.spawn(browser, {}, async function() {
    let childdoc = content.document.getElementsByTagName("iframe")[0].contentDocument;
    Assert.equal(childdoc.activeElement, childdoc.getElementById("i"),
      "First invalid element should be focused");
  });

  ok(gInvalidFormPopup.state == "showing" || gInvalidFormPopup.state == "open",
     "The invalid form popup should be shown");

  gBrowser.removeCurrentTab();
});

