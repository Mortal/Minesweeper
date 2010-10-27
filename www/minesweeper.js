var MineSweeper = Base.extend({
	dom: null,
	iconwidth: 16,
	iconheight: 16,
	frames: null,
	imgclass_cb: null,
	frameclass: null,
	defaultchar: ' ',
	warn: function () {
		if ((typeof console) != 'undefined' && console.warn) console.warn(arguments);
	},
	constructor: function (dom, imgclass_cb) {
		this.dom = dom;
		this.frames = [];
		this.imgclass_cb = imgclass_cb;
	},
	pushFrame: function (text) {
		return this.parseFrame(this.frames.length, text);
	},
	parseFrame: function (frameindex, text) {
		var rows = [[]];
		var currow = rows[0];
		for (var i = 0; i < text.length; i++) {
			var c = text.charAt(i);
			if (c == '\r') {
				if (text.charAt(i+1) == '\n') ++i;
				currow = [];
				rows.push(currow);
			} else if (c == '\n') {
				currow = [];
				rows.push(currow);
			} else {
				currow.push(c);
			}
		}
		this.frames[frameindex] = rows;
	},
	deltaficate: function (frameindex) {
		var rows = this.frames[frameindex];
		if (!this.frames[frameindex-1]) return;
		var oldrows = this.frames[frameindex-1];
		for (var y = 0; y < oldrows.length; ++y) {
			var oldrow = oldrows[y];
			while (rows.length <= y) {
				rows.push([]);
			}
			var row = rows[y];
			for (var x = 0; x < oldrow.length; ++x) {
				while (row.length <= x) row.push(this.defaultchar);
				if (row[x] == oldrow[x]) row[x] = false;
			}
		}
	},
	deltaficateAll: function () {
		for (var i = this.frames.length; i--;) this.deltaficate(i);
	},
	drawFrame: function (sb, frameindex) {
		if (!this.frames[frameindex]) {sb.push('<!-- no such frame -->'); return;}
		var rows = this.frames[frameindex];
		for (var y = 0; y < rows.length; ++y) {
			var row = rows[y];
			for (var x = 0; x < row.length; ++x) {
				if (row[x] === false) continue;
				sb.push('<div class="');
				this.imgclass_cb(sb, row[x]);
				sb.push('" style="top:',(y*this.iconheight),'px;left:',(x*this.iconwidth),'px;"><\/div>');
			}
		}
	},
	drawFrames: function (sb) {
		for (var i = 0; i < this.frames.length; ++i) {
			sb.push('<div');
			if (this.frameclass) sb.push(' class="',this.frameclass,'"');
			else sb.push(' style="position:relative"');
			sb.push('>');
			this.drawFrame(sb, i);
			sb.push('<\/div>');
		}
	},
	debugoutputframe: function (sb, i) {
		var rows = this.frames[i];
		for (var y = 0; y < rows.length; ++y) {
			var row = rows[y];
			for (var x = 0; x < row.length; ++x) {
				sb.push(row[x] ? row[x] : ',');
			}
			sb.push(';\n');
		}
	}
});
