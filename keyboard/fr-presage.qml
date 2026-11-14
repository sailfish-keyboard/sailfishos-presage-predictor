/*
 * French (AZERTY) keyboard layout with Presage text prediction.
 *
 * Based on the layout conventions of the other presage keyboards in this
 * package: accented letters are available through long-press accent lists,
 * and the most frequent French accented letters (é, ç) get dedicated keys.
 */

import QtQuick 2.0
import ".."

KeyboardLayout {
    splitSupported: true

    KeyboardRow {
        AccentedCharacterKey {
            caption: "a"; captionShifted: "A"; symView: "1"; symView2: "`"
            accents: "aàâä"; accentsShifted: "AÀÂÄ"
            nativeAccents: "à"; nativeAccentsShifted: "À"
        }
        CharacterKey { caption: "z"; captionShifted: "Z"; symView: "2"; symView2: "£" }
        AccentedCharacterKey {
            caption: "e"; captionShifted: "E"; symView: "3"; symView2: "$"
            accents: "eéèêë"; accentsShifted: "ÉÈÊËE"
            nativeAccents: "é"; nativeAccentsShifted: "É"
        }
        CharacterKey { caption: "r"; captionShifted: "R"; symView: "4"; symView2: "¥" }
        CharacterKey { caption: "t"; captionShifted: "T"; symView: "5"; symView2: "₹" }
        CharacterKey { caption: "y"; captionShifted: "Y"; symView: "6"; symView2: "%" }
        AccentedCharacterKey {
            caption: "u"; captionShifted: "U"; symView: "7"; symView2: "<"
            accents: "uûùü"; accentsShifted: "UÛÙÜ"
            nativeAccents: "û"; nativeAccentsShifted: "Û"
        }
        AccentedCharacterKey {
            caption: "i"; captionShifted: "I"; symView: "8"; symView2: ">"
            accents: "iîï"; accentsShifted: "ÎÏI"
            nativeAccents: "î"; nativeAccentsShifted: "Î"
        }
        AccentedCharacterKey {
            caption: "o"; captionShifted: "O"; symView: "9"; symView2: "["
            accents: "oôöø"; accentsShifted: "ÔÖØO"
            nativeAccents: "ô"; nativeAccentsShifted: "Ô"
        }
        CharacterKey { caption: "p"; captionShifted: "P"; symView: "0"; symView2: "]" }
        AccentedCharacterKey {
            caption: "é"; captionShifted: "É"; symView: "!"; symView2: "¡"
            accents: "éèêëe"; accentsShifted: "ÉÈÊËE"
            nativeAccents: "é"; nativeAccentsShifted: "É"
        }
    }

    KeyboardRow {
        CharacterKey { caption: "q"; captionShifted: "Q"; symView: "*"; symView2: "^" }
        CharacterKey { caption: "s"; captionShifted: "S"; symView: "#"; symView2: "&" }
        CharacterKey { caption: "d"; captionShifted: "D"; symView: "+"; symView2: "|" }
        CharacterKey { caption: "f"; captionShifted: "F"; symView: "-"; symView2: "_" }
        CharacterKey { caption: "g"; captionShifted: "G"; symView: "="; symView2: "§" }
        CharacterKey { caption: "h"; captionShifted: "H"; symView: "("; symView2: "{" }
        CharacterKey { caption: "j"; captionShifted: "J"; symView: ")"; symView2: "}" }
        CharacterKey { caption: "k"; captionShifted: "K"; symView: "\""; symView2: "°" }
        CharacterKey { caption: "l"; captionShifted: "L"; symView: "~"; symView2: "·" }
        CharacterKey { caption: "m"; captionShifted: "M"; symView: ":"; symView2: "×" }
        AccentedCharacterKey {
            caption: "ç"; captionShifted: "Ç"; symView: "?"; symView2: "¿"
            accents: "cç"; accentsShifted: "CÇ"
            nativeAccents: "ç"; nativeAccentsShifted: "Ç"
        }
    }

    KeyboardRow {
        splitIndex: 5

        ShiftKey {}

        CharacterKey { caption: "w"; captionShifted: "W"; symView: "@"; symView2: "«" }
        CharacterKey { caption: "x"; captionShifted: "X"; symView: "&"; symView2: "»" }
        AccentedCharacterKey {
            caption: "c"; captionShifted: "C"; symView: "/"; symView2: "÷"
            accents: "cç"; accentsShifted: "CÇ"
            nativeAccents: "ç"; nativeAccentsShifted: "Ç"
        }
        CharacterKey { caption: "v"; captionShifted: "V"; symView: "\\"; symView2: "“" }
        CharacterKey { caption: "b"; captionShifted: "B"; symView: "'"; symView2: "”" }
        CharacterKey { caption: "n"; captionShifted: "N"; symView: ";"; symView2: "„" }
        // Apostrophe gets the slot right of N (the old ,/< key): comma is already
        // available left of the spacebar and < is rarely needed, while ' is essential
        // in French. Period and > stay reachable through this key's symbol views.
        CharacterKey { caption: "'"; captionShifted: "\""; symView: "."; symView2: ">" }

        BackspaceKey {}
    }

    SpacebarRow {}
}
