/* Copyright (C) 2014-2015 Dan Chapman <dpniel@ubuntu.com>

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
import QtQuick 2.4
import QtTest 1.0
import MailAutoDiscover 1.0

TestCase {
    name: "EmailValidatorTests"

    function test_valid_mailaddress_data() {
        return [
                    {tag: "basic", addr: "fred@example.com"},
                    {tag: "Quoted Mixed", addr: "\"Fred Bloggs\"@example.com"},
                    {tag: "Quoted with escaped", addr: "\"Joe\\\\Blow\"@example.com"},
                    {tag: "Quoted with @", addr: "\"Abc@def\"@example.com"},
                    {tag: "Random valid", addr: "customer/department=shipping@example.com"},
                    {tag: "Numerical", addr: "12345@example.com"},
                    {tag: "crazy", addr: "!def!xyz%abc@example.com"},
                    {tag: "leading _", addr: "_somename@example.com"},
                    {tag: "IPv4", addr: "valid.ipv4.addr@[123.1.72.10]"},
                    {tag: "IPv6", addr: "valid.ipv6.addr@[IPv6:0::1]"},
                    {tag: "simple", addr: "niceandsimple@example.com"},
                    {tag: "common", addr: "very.common@example.com"},
                    {tag: "disposable", addr: "a.little.lengthy.but.fine@dept.example.com"},
                    {tag: "unusual", addr: "disposable.style.email.with+symbol@example.com"},
                    {tag: "more unusual", addr: "\"much.more unusual\"@example.com"},
                    {tag: "very unusual", addr: "\"very.unusual.@.unusual.com\"@example.com"},
                    {tag: "freakin weird but still valid", addr: "\"very.(),:;<>[]\\\".VERY.\\\"very@\\\\ \\\"very\\\".unusual\"@strange.example.com"},
                    {tag: "different", addr: "postbox@com"},
                    {tag: "admin", addr: "admin@mailserver1"},
                    {tag: "Atom characters", addr: "!#$%&'*+-/=?^_`{}|~@example.org"},
                    {tag: "Atom character 2", addr: "\"()<>[]:,;@\\\\\\\"!#$%&'*+-/=?^_`{}| ~.a\"@example.org"},
                    {tag: "Empty quote", addr: "\" \"@example.org"}
                ]
    }

    function test_invalid_mailaddress_data() {
        return [
                    {tag: "invalid1", addr: "invalid"},
                    {tag: "invalid2", addr: "invalid@"},
                    {tag: "invalid3", addr: "invalid @"},
                    {tag: "invalid4", addr: "invalid@[555.666.777.888]"},
                    {tag: "invalid5", addr: "invalid@[IPv6:123456]"},
                    {tag: "invalid6", addr: "invalid@[127.0.0.1.]"},
                    {tag: "invalid7", addr: "invalid@[127.0.0.1]x"},
                    {tag: "invalid8", addr: "_somename@example@com"},
                    {tag: "invalid9", addr: "Abc.example.com"},
                    {tag: "invalid10", addr: "A@b@c@example.com"},
                    {tag: "invalid11", addr: "a\"b(c)d,e:f;g<h>i[j\\k]l@example.com"},
                    {tag: "invalid12", addr: "just\"not\"right@example.com"},
                    {tag: "invalid13", addr: "this is\"not\\allowed@example.com"},
                    {tag: "invalid14", addr: "this\\ still\\\"not\\\\allowed@example.com"},
                    {tag: "No longer available", addr: "No longer available."},
                    {tag: "moved", addr: "Moved."}
               ]
    }

    function test_valid_mailaddress(data) {
        compare(EmailValidator.validate(data.addr), true, data.tag)
    }

    function test_invalid_mailaddress(data) {
        compare(EmailValidator.validate(data.addr), false, data.tag)
    }
}
