(function (Module) {
	'use strict';

	Module.libraries.push({
		name: 'mm',
		init: function (instance) {
			instance.exports.mm_init();
		}
	});

})(Module);
