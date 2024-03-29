/*
 * Copyright (C) 2017 Jolla ltd and/or its subsidiary(-ies). All rights reserved.
 *
 * Contact: Pekka Vuorela <pekka.vuorela@jollamobile.com>
 * Contact: Timur Kristóf <timur.kristof@gmail.com>
 * Contact: Miklos Marton <miklos.marton@ni.com>
 * Contact: Simonas Leleiva <simonas.leleiva@jollamobile.com>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * Neither the name of Jolla Ltd nor the names of its contributors may be
 * used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

import QtQuick 2.0
import ".."

KeyboardLayout {
    splitSupported: true

    KeyboardRow {
        CharacterKey { caption: "q"; captionShifted: "Q"; symView: "0"; symView2: "€" }
        CharacterKey { caption: "w"; captionShifted: "W"; symView: "1"; symView2: "£" }
        AccentedCharacterKey {
            caption: "e"
            captionShifted: "E"
            symView: "2"
            symView2: "$"
            accents: "€eéë"
            accentsShifted: "€EÉË"
            deadKeyAccents: "´é"
            deadKeyAccentsShifted: "´É"
        }
        CharacterKey { caption: "r"; captionShifted: "R"; symView: "3"; symView2: "¥" }
        CharacterKey { caption: "t"; captionShifted: "T"; symView: "4"; symView2: "₹"; accents: "tþ"; accentsShifted: "TÞ" }
        CharacterKey { caption: "z"; captionShifted: "Z"; symView: "5"; symView2: "¤" }
        AccentedCharacterKey {
            caption: "u"
            captionShifted: "U"
            symView: "6"
            symView2: "<"
            accents: "űuüú"
            accentsShifted: "ŰUÜÚ"
            deadKeyAccents: "´ú"
            deadKeyAccentsShifted: "´Ú"
        }
        AccentedCharacterKey {
            caption: "i"
            captionShifted: "I"
            symView: "7"
            symView2: ">"
            accents: "ïií"
            accentsShifted: "ÏIÍ"
            deadKeyAccents: "´í"
            deadKeyAccentsShifted: "´Í"
        }
        AccentedCharacterKey {
            caption: "o"
            captionShifted: "O"
            symView: "8"
            symView2: "["
            accents: "óőoö"
            accentsShifted: "ÓŐOÖ"
            deadKeyAccents: "´ó"
            deadKeyAccentsShifted: "´Ó"
        }
        CharacterKey { caption: "p"; captionShifted: "P"; symView: "9"; symView2: "]" }
        AccentedCharacterKey {
            caption: "ö"
            captionShifted: "Ö"
            symView: "%"
            symView2: "‰"
            fontSizeMode: Text.HorizontalFit
            accents: "őóö"
            accentsShifted: "ŐÓÖ"
            deadKeyAccents: "´ő"
            deadKeyAccentsShifted: "´Ő"
        }
    }

    KeyboardRow {
        AccentedCharacterKey {
            caption: "a"
            captionShifted: "A"
            symView: "*"
            symView2: "`"
            accents: "aáä"
            accentsShifted: "AÁÄ"
            deadKeyAccents: "´á"
            deadKeyAccentsShifted: "´Á"
        }
        CharacterKey { caption: "s"; captionShifted: "S"; symView: "#"; symView2: "^"; accents: "sß$"; accentsShifted: "S$" }
        CharacterKey { caption: "d"; captionShifted: "D"; symView: "+"; symView2: "|"; accents: "dð"; accentsShifted: "DÐ" }
        CharacterKey { caption: "f"; captionShifted: "F"; symView: "-"; symView2: "_" }
        CharacterKey { caption: "g"; captionShifted: "G"; symView: "="; symView2: "§" }
        CharacterKey { caption: "h"; captionShifted: "H"; symView: "("; symView2: "{" }
        CharacterKey { caption: "j"; captionShifted: "J"; symView: ")"; symView2: "}" }
        CharacterKey { caption: "k"; captionShifted: "K"; symView: "\""; symView2: "°" }
        CharacterKey { caption: "l"; captionShifted: "L"; symView: "!"; symView2: "¡" }
        AccentedCharacterKey {
            caption: "ü"
            captionShifted: "Ü"
            symView: "?"
            symView2: "¿"
            accents: "űúü"
            accentsShifted: "ŰÚÜ"
            deadKeyAccents: "´ű"
            deadKeyAccentsShifted: "´Ű"
        }
        DeadKey { caption: "´"; captionShifted: "´" }
    }

    KeyboardRow {
        splitIndex: 5

        ShiftKey {}

        CharacterKey { caption: "y"; captionShifted: "Y"; symView: "@"; symView2: "«"; accents: "yý¥"; accentsShifted: "YÝ¥" }
        CharacterKey { caption: "x"; captionShifted: "X"; symView: "&"; symView2: "»" }
        CharacterKey { caption: "c"; captionShifted: "C"; symView: "/"; symView2: "÷"; accents: "cç"; accentsShifted: "CÇ" }
        CharacterKey { caption: "v"; captionShifted: "V"; symView: "\\"; symView2: "“" }
        CharacterKey { caption: "b"; captionShifted: "B"; symView: "'"; symView2: "”" }
        CharacterKey { caption: "n"; captionShifted: "N"; symView: ";"; symView2: "„"; accents: "nñ"; accentsShifted: "NÑ" }
        CharacterKey { caption: "m"; captionShifted: "M"; symView: ":"; symView2: "×" }

        BackspaceKey {}
    }

    SpacebarRow {}
}
