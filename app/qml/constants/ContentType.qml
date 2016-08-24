/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
pragma Singleton
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Content 1.1 as CH

Item {

    function resolveType(fileUrl) {
        console.log("resolveContentType for file", fileUrl)
        var extension = d.__fileExtension(fileUrl)
        extension = extension.toLowerCase()
        console.log("file extension:", extension)
        var contentType = d.contentMap[extension]

        if (contentType === undefined) {
            console.log("Unrecognized extension", extension)
            contentType = CH.ContentType.All
        }

        console.log("returning contentType:", contentType)

        return contentType
    }

    QtObject {
        id: d

        function __fileExtension(fileUrl) {
            var lastDotIndex = fileUrl.lastIndexOf('.')
            return lastDotIndex > -1 ? fileUrl.substring(lastDotIndex + 1) : ''
        }

        property var contentMap: {
            'png': CH.ContentType.Pictures,
            'jpg': CH.ContentType.Pictures,
            'jpeg': CH.ContentType.Pictures,
            'bmp': CH.ContentType.Pictures,
            'gif': CH.ContentType.Pictures,

            'mp3': CH.ContentType.Music,
            'ogg': CH.ContentType.Music,
            'wav': CH.ContentType.Music,
            'm4a': CH.ContentType.Music,
            'opus': CH.ContentType.Music,
            'flac': CH.ContentType.Music,

            'avi': CH.ContentType.Videos,
            'mpeg': CH.ContentType.Videos,
            'mp4': CH.ContentType.Videos,
            'mkv': CH.ContentType.Videos,
            'mov': CH.ContentType.Videos,
            'wmv': CH.ContentType.Videos,

            'txt': CH.ContentType.Documents,
            'doc': CH.ContentType.Documents,
            'docx': CH.ContentType.Documents,
            'xls': CH.ContentType.Documents,
            'xlsx': CH.ContentType.Documents,
            'ppt': CH.ContentType.Documents,
            'pptx': CH.ContentType.Documents,
            'pdf': CH.ContentType.Documents,
            'odt': CH.ContentType.Documents,
            'ott': CH.ContentType.Documents,
            'oth': CH.ContentType.Documents,
            'ods': CH.ContentType.Documents,
            'ots': CH.ContentType.Documents,
            'html': CH.ContentType.Documents,
            'odp': CH.ContentType.Documents,
            'otp': CH.ContentType.Documents,
            'odf': CH.ContentType.Documents,
            'odg': CH.ContentType.Documents,
            'otg': CH.ContentType.Documents,
            'odm': CH.ContentType.Documents,

            'vcard': CH.ContentType.Contacts,
            'vcf': CH.ContentType.Contacts
        }
    }
}
