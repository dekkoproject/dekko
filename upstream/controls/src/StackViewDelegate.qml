import QtQuick 2.4

QtObject {
    id: root

    /*!
        \qmlmethod Transition StackViewDelegate::getTransition(properties)

        The base implementation of this function just looks for a property named properties.name inside itself.
        \sa {Transitions}
    */
    function getTransition(properties)
    {
        return root[properties.name]
    }

    /*!
        \qmlmethod void StackViewDelegate::transitionFinished(properties)

        The base implementation of this function is empty.
        \sa {Transitions}
    */
    function transitionFinished(properties)
    {
    }

    /*!
        \qmlproperty Component StackViewDelegate::pushTransition

        The transition used on push operation.
    */
    property Component pushTransition: StackViewTransition {}
    /*!
        \qmlproperty Component StackViewDelegate::popTransition

        The transition used on pop operation.
        Unless set, the popTransition is the same as pushTransition
    */
    property Component popTransition: root["pushTransition"]
    /*!
        \qmlproperty Component StackViewDelegate::replaceTransition

        The transition used on replace operation.
        Unless set, the replaceTransition is the same as pushTransition
    */
    property Component replaceTransition: root["pushTransition"]
}

