/***
    
MochiKit.Mockdom 1.3.1

See <http://mochikit.com/> for documentation, downloads, license, etc.
    
(c) 2005 Bob Ippolito.  All rights Reserved.

***/
if (typeof(MochiKit) == "undefined") {
    var MochiKit = {};
}

if (typeof(MochiKit.Mockdom) == "undefined") {
    MochiKit.Mockdom = {};
}

MochiKit.Mockdom.NAME = "MochiKit.Mockdom";
MochiKit.Mockdom.VERSION = "1.3.1";

MochiKit.Mockdom.__repr__ = function () {
    return "[" + this.NAME + " " + this.VERSION + "]";
};

MochiKit.Mockdom.toString = function () {
    return this.__repr__();
};

MochiKit.Mockdom.createDocument = function () {
    var doc = new MochiKit.Mockdom.MockElement("DOCUMENT");
    doc.body = doc.createElement("BODY");
    doc.appendChild(doc.body);
    return doc;
};

MochiKit.Mockdom.MockElement = function (name, data) {
    this.nodeName = name.toUpperCase();
    if (typeof(data) == "string") {
        this.nodeValue = data;
        this.nodeType = 3;
    } else {
        this.nodeType = 1;
        this.childNodes = [];
    }
    if (name.substring(0, 1) == "<") {
        var nameattr = name.substring(
            name.indexOf('"') + 1, name.lastIndexOf('"'));
        name = name.substring(1, name.indexOf(" "));
        this.nodeName = name.toUpperCase();
        this.setAttribute("name", nameattr);
    }
};

MochiKit.Mockdom.MockElement.prototype = {
    createElement: function (nodeName) {
        return new MochiKit.Mockdom.MockElement(nodeName);
    },
    createTextNode: function (text) {
        return new MochiKit.Mockdom.MockElement("text", text);
    },
    setAttribute: function (name, value) {
        this[name] = value;
    },
    getAttribute: function (name) {
        return this[name];
    },
    appendChild: function (child) {
        this.childNodes.push(child);
    },
    toString: function () {
        return "MockElement(" + this.nodeName + ")";
    }
};
