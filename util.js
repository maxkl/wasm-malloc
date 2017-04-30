var util = (function () {
	'use strict';

	var util = {};

	var utf8Decoder = new TextDecoder('utf8');

	util.ptr2str = function (buffer, ptr) {
		var raw = new Uint8Array(buffer, ptr);
		var nul = raw.indexOf(0);
		if(nul !== -1) {
			raw = raw.subarray(0, nul);
		}
		return utf8Decoder.decode(raw);
	};

	return util;
})();