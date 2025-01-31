/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * Main module handling references to objects living in the main process.
 */

"use strict";

this.EXPORTED_SYMBOLS = [
  "FormAutofill",
];

const { classes: Cc, interfaces: Ci, utils: Cu, results: Cr } = Components;

Cu.import("resource://gre/modules/Integration.jsm");

/**
 * Main module handling references to objects living in the main process.
 */
this.FormAutofill = {
  /**
   * Registers new overrides for the FormAutofillIntegration methods.  Example:
   *
   *   FormAutofill.registerIntegration(base => ({
   *     createRequestAutocompleteUI: Task.async(function* () {
   *       yield base.createRequestAutocompleteUI.apply(this, arguments);
   *     }),
   *   }));
   *
   * @param aIntegrationFn
   *        Function returning an object defining the methods that should be
   *        overridden.  Its only parameter is an object that contains the base
   *        implementation of all the available methods.
   *
   * @note The integration function is called every time the list of registered
   *       integration functions changes.  Thus, it should not have any side
   *       effects or do any other initialization.
   */
  registerIntegration(aIntegrationFn) {
    Integration.formAutofill.register(aIntegrationFn);
  },

  /**
   * Removes a previously registered FormAutofillIntegration override.
   *
   * Overrides don't usually need to be unregistered, unless they are added by a
   * restartless add-on, in which case they should be unregistered when the
   * add-on is disabled or uninstalled.
   *
   * @param aIntegrationFn
   *        This must be the same function object passed to registerIntegration.
   */
  unregisterIntegration(aIntegrationFn) {
    Integration.formAutofill.unregister(aIntegrationFn);
  },

  /**
   * Processes a requestAutocomplete message asynchronously.
   *
   * @param aData
   *        Provided to FormAutofillIntegration.createRequestAutocompleteUI.
   *
   * @return {Promise}
   * @resolves Structured data received from the requestAutocomplete UI.
   */
  async processRequestAutocomplete(aData) {
    let ui = await FormAutofill.integration.createRequestAutocompleteUI(aData);
    return await ui.show();
  },
};

/**
 * Dynamically generated object implementing the FormAutofillIntegration
 * methods.  Platform-specific code and add-ons can override methods of this
 * object using the registerIntegration method.
 */
Integration.formAutofill.defineModuleGetter(
  this.FormAutofill,
  "integration",
  "resource://gre/modules/FormAutofillIntegration.jsm",
  "FormAutofillIntegration"
);
