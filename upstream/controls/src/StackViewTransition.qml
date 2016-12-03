import QtQuick 2.4

ParallelAnimation {
    id: root
    /*! The name of the animation that is running. Can be one of the following:
    \list
    \li 'PushTransition'
    \li 'PopTransition'
    \li 'ReplaceTransition'
    \endlist
    */
    property string name
    /*! The page that is transitioning in. */
    property Item enterItem
    /*! The page that is transitioning out */
    property Item exitItem
    /*! Set to \c true if the transition is told to
        fast-forward directly to its end-state */
    property bool immediate
}

