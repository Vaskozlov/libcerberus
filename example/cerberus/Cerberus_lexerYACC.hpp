cerb::Map<CerberusLexerItem, yytokentype> CerberusLexerItemsNamesConverter(
    {
        {CerberusLexerItem::UNDEFINED           , yytokentype::UNDEFINED},
        {CerberusLexerItem::EoF                 , yytokentype::EoF},
        {CerberusLexerItem::ADD                 , yytokentype::ADD},
        {CerberusLexerItem::SUB                 , yytokentype::SUB},
        {CerberusLexerItem::MUL                 , yytokentype::MUL},
        {CerberusLexerItem::DIV                 , yytokentype::DIV},
        {CerberusLexerItem::MOD                 , yytokentype::MOD},
        {CerberusLexerItem::ASSIGN              , yytokentype::ASSIGN},
        {CerberusLexerItem::ADD_EQ              , yytokentype::ADD_EQ},
        {CerberusLexerItem::SUB_EQ              , yytokentype::SUB_EQ},
        {CerberusLexerItem::MUL_EQ              , yytokentype::MUL_EQ},
        {CerberusLexerItem::DIV_EQ              , yytokentype::DIV_EQ},
        {CerberusLexerItem::MOD_EQ              , yytokentype::MOD_EQ},
        {CerberusLexerItem::OR                  , yytokentype::OR},
        {CerberusLexerItem::AND                 , yytokentype::AND},
        {CerberusLexerItem::XOR                 , yytokentype::XOR},
        {CerberusLexerItem::NOT                 , yytokentype::NOT},
        {CerberusLexerItem::OR_EQ               , yytokentype::OR_EQ},
        {CerberusLexerItem::AND_EQ              , yytokentype::AND_EQ},
        {CerberusLexerItem::XOR_EQ              , yytokentype::XOR_EQ},
        {CerberusLexerItem::LEFT_SHIFT          , yytokentype::LEFT_SHIFT},
        {CerberusLexerItem::RIGHT_SHIFT         , yytokentype::RIGHT_SHIFT},
        {CerberusLexerItem::LEFT_SHIFT_EQ       , yytokentype::LEFT_SHIFT_EQ},
        {CerberusLexerItem::RIGHT_SHIFT_EQ      , yytokentype::RIGHT_SHIFT_EQ},
        {CerberusLexerItem::FOR                 , yytokentype::FOR},
        {CerberusLexerItem::WHILE               , yytokentype::WHILE},
        {CerberusLexerItem::RETURN              , yytokentype::RETURN},
        {CerberusLexerItem::CASE                , yytokentype::CASE},
        {CerberusLexerItem::SWITCH              , yytokentype::SWITCH},
        {CerberusLexerItem::FUNC                , yytokentype::FUNC},
        {CerberusLexerItem::VAR                 , yytokentype::VAR},
        {CerberusLexerItem::LESS                , yytokentype::LESS},
        {CerberusLexerItem::GREATER             , yytokentype::GREATER},
        {CerberusLexerItem::LOGICAL_NOT         , yytokentype::LOGICAL_NOT},
        {CerberusLexerItem::LESS_EQ             , yytokentype::LESS_EQ},
        {CerberusLexerItem::GREATER_EQ          , yytokentype::GREATER_EQ},
        {CerberusLexerItem::LOGICAL_OR          , yytokentype::LOGICAL_OR},
        {CerberusLexerItem::LOGICAL_AND         , yytokentype::LOGICAL_AND},
        {CerberusLexerItem::LOGICAL_EQUAL       , yytokentype::LOGICAL_EQUAL},
        {CerberusLexerItem::LOGICAL_NOT_EQUAL   , yytokentype::LOGICAL_NOT_EQUAL},
        {CerberusLexerItem::CURLY_OPENING       , yytokentype::CURLY_OPENING},
        {CerberusLexerItem::CURLY_CLOSING       , yytokentype::CURLY_CLOSING},
        {CerberusLexerItem::LEFT_PARENTHESIS    , yytokentype::LEFT_PARENTHESIS},
        {CerberusLexerItem::RIGHT_PARENTHESIS   , yytokentype::RIGHT_PARENTHESIS},
        {CerberusLexerItem::ANGLE_OPENING       , yytokentype::ANGLE_OPENING},
        {CerberusLexerItem::ANGLE_CLOSING       , yytokentype::ANGLE_CLOSING},
        {CerberusLexerItem::COMMA               , yytokentype::COMMA},
        {CerberusLexerItem::SEPARATOR           , yytokentype::SEPARATOR},
        {CerberusLexerItem::PREPROCESSOR        , yytokentype::PREPROCESSOR},
        {CerberusLexerItem::QUESTION_MARK       , yytokentype::QUESTION_MARK},
        {CerberusLexerItem::ARROW               , yytokentype::ARROW},
        {CerberusLexerItem::STMT                , yytokentype::STMT},
        {CerberusLexerItem::GLOBAL_VIEW         , yytokentype::GLOBAL_VIEW},
        {CerberusLexerItem::INT_T               , yytokentype::TYPE},
        {CerberusLexerItem::CHAR_T              , yytokentype::TYPE},
        {CerberusLexerItem::FLOAT_T             , yytokentype::TYPE},
        {CerberusLexerItem::DOUBLE_T            , yytokentype::TYPE},
        {CerberusLexerItem::BUILTIN_AUTO_T      , yytokentype::TYPE},
        {CerberusLexerItem::TYPE                , yytokentype::TYPE},
        {CerberusLexerItem::INTEGER             , yytokentype::INTEGER},
        {CerberusLexerItem::FLOAT               , yytokentype::FLOAT},
        {CerberusLexerItem::DOUBLE              , yytokentype::DOUBLE},
        {CerberusLexerItem::IDENTIFIER          , yytokentype::IDENTIFIER},
        {CerberusLexerItem::CHAR                , yytokentype::CHAR},
        {CerberusLexerItem::STRING              , yytokentype::STRING},
    }
);
