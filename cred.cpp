#include "cred.h"


ValidationResult Cred::validate(const QString &service, const QString &username, const QString &password) {
    ValidationResult validated;
    QStringList message;
    validated.result = true;

    if (service.trimmed().isEmpty()) {
        validated.result = false;
        message << "Nazwa serwisu nie może być pusta";
    } else if (service.length() > 1024) {
        validated.result = false;
        message << "Nazwa serwisu nie może być dłuższa niż 1024 znaki";
    }

    if (username.trimmed().isEmpty()) {
        validated.result = false;
        message << "Nazwa użytkownika nie może być pusta";
    } else if (username.contains(" ")) {
        validated.result = false;
        message << "Spacja nie może znajdować się w nazwie użytkownika";
    } else if (service.length() > 1024) {
        validated.result = false;
        message << "Nazwa użytkownika nie może być dłuższa niż 1024 znaki";
    }

    if (password.trimmed().isEmpty()) {
        validated.result = false;
        message << "Hasło nie może być puste";
    } else if (service.length() > 2048) {
        validated.result = false;
        message << "Hasło nie może być dłuższe niż 2048 znaki";
    }

    if (validated.isVaid())
        validated.message = "ok";
    else
        validated.message = "•" + message.join("\n•");

    return validated;
}

PasswordStrength::PasswordStrength(const QString &password) : score(0) {

    if (password.length() >= 8) {
        this->score += 10;
    } else {
        this->suggestions << "Hasło powinno zawierać co najmniej 8 znaków";
    }

    score += qMax(0, qMin((password.length() - 8) * 2, 65));

    bool has_lower = false, has_upper = false, has_digit = false, has_symbol = false;
    for(const auto &c : password) {
        if (c.isLower()) has_lower = true;
        if (c.isUpper()) has_upper = true;
        if (c.isDigit()) has_digit = true;
        if (!c.isLetterOrNumber()) has_symbol = true;
    }
    if (has_lower)
        this->score += 5;
    else
        this->suggestions << "Hasło powinno zawierać minuskułę";

    if (has_upper)
        this->score += 5;
    else
        this->suggestions << "Hasło powinno zawierać majuskułę";

    if (has_digit)
        this->score += 5;
    else
        this->suggestions << "Hasło powinno zawierać cyfrę";

    if (has_symbol)
        this->score += 10;
    else
        this->suggestions << "Hasło powinno zawierać znak specjalny";

    if (score < Ratings::WEAK)       this->rating = "Słabe";
    else if (score < Ratings::MEDIUM)  this->rating = "Średnie";
    else if (score < Ratings::GOOD)  this->rating = "Mocne";
    else                  this->rating = "Bardzo mocne";
}
