import QtQuick 2.4
import MazDB 1.0

MazDBListModel {

    property var collection: undefined

    source: collection ? collection.source : ""

    range {
        start: collection ? collection.startKey : ""
        end: collection ? collection.endKey : ""
    }

    filter: collection ? collection.name : ""

    filterPolicy: MazDBListModel.StartsWith

    excludeKeys: collection ? [collection.startKey, collection.endKey] : []

    customRoles: [
        // Version of this dCard
        "version",
        // Fullname value
        "fullname",
        // Name object
        "name",
        // Formatted name based on either fullname or name objects
        "displayName",
        // Birthday as a Date() object
        "bday",
        // Anniversary object
        "anniversary",
        // Gender object
        "gender",
        // Organizations array
        "organizations",
        // Organization with either pref: 1 or first in array
        "displayOrg",
        // Address array
        "addresses",
        "prefferedAddress",
        // Tel array
        "telephones",
        // Phone with either pref: 1 or first in array
        "displayPhone",
        // emails array
        "emails",
        // email with pref1 or first in array
        "displayEmail",
        // timezone object
        "timezone",
        // Urls array
        "urls"
    ]

    rolesCallBack: function (role, key, data) {
        switch(role) {
        case "version":
            return data.version;
        case "fullname":
            return data.fn;
        case "name":
            return data.n;
        case "displayName":
            if (data.fn.value) {
                return data.fn.value;
            } else if (data.n.firstname) {
                return "%1 %2".arg(data.n.firstname).arg(data.n.surname);
            } else if (data.n.surname) {
                return data.n.surname;
            } else {
                return qsTr("Unknown");
            }
        case "bday":
            return new Date(data.bday.value)
        case "anniversary":
            return new Date(data.anniversary.value)
        case "gender":
            return data.gender;
        case "organization":
            return data.org
        case "displayOrg":
            return _getPrefferedOrFirst(data.org)
        case "addresses":
            return data.adr;
        case "prefferedAddress":
            return _getPrefferedOrFirst(data.arg)
        case "telephones":
            return data.tel
        case "displayPhone":
            return _getPrefferedOrFirst(data.tel)
        case "emails":
            return data.email
        case "displayEmail":
            return _getPrefferedOrFirst(data.email)
        case "timezone":
            return data.tz;
        case "urls":
            return data.url
        default:
            return undefined
        }
    }

    function _getPrefferedOrFirst(obj) {
        for (var i in obj) {
            var v = obj[i]
            if (v.pref && v.pref === "1") {
                return v
            }
        }
        return obj[0]
    }
}
