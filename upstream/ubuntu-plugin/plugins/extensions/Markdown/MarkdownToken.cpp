#include "MarkdownToken.h"

MarkdownToken::MarkdownToken() :
    m_type(TokenType::Unknown),
    m_position(0),
    m_length(0),
    m_opening(0),
    m_closing(0){}

MarkdownToken::~MarkdownToken(){}

MarkdownToken::TokenType MarkdownToken::type() const { return m_type; }

void MarkdownToken::setType(MarkdownToken::TokenType t) { m_type = t; }

int MarkdownToken::position() const { return m_position; }

int MarkdownToken::length() const { return m_length; }

void MarkdownToken::setLength(int len) { m_length = len; }

int MarkdownToken::openingLength() const { return m_opening; }

void MarkdownToken::setOpeningLenth(int len) { m_opening = len; }

int MarkdownToken::closingLength() const { return m_closing; }

void MarkdownToken::setClosingLength(int len) { m_closing = len; }
