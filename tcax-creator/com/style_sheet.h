#ifndef STYLE_SHEET_H
#define STYLE_SHEET_H

namespace StyleSheet
{
    #define folderTree \
    "QTreeView" \
    "{" \
    "border: 1px solid rgb(50, 50, 50);" \
    "background-color: rgb(255, 255, 255);" \
    "}" \
    "QTreeView::item" \
    "{" \
    "height: 25px;" \
    "color: white;" \
    "border: none;" \
    "border-bottom: 1px solid rgb(255, 255, 255);" \
    "background-color: transparent;" \
    "}" \
    "QTreeView::item:hover" \
    "{" \
    "background-color: rgba(255, 255, 255, 40);" \
    "}" \
    "QTreeView::item:selected" \
    "{" \
    "background-color: rgb(0, 160, 230);" \
    "}" \
    "QTreeView::branch:open:has-children" \
    "{" \
    "image: url(:/qss/branch-open.png);" \
    "}" \
    "QTreeView::branch:closed:has-children" \
    "{" \
    "image: url(:/qss/branch-closed.png);" \
    "}" \
    "QTreeView::indicator" \
    "{" \
    "width: 20px;" \
    "height: 20px;" \
    "}" \
    "QTreeView::indicator:enabled:unchecked" \
    "{" \
    "	" \
    "image: url(:/Images/checkBox);" \
    "}" \
    "QTreeView::indicator:enabled:unchecked:hover" \
    "{" \
    "image: url(:/Images/checkBoxHover);" \
    "}" \
    "QTreeView::indicator:enabled:unchecked:pressed" \
    "{" \
    "image: url(:/Images/checkBoxPressed);" \
    "}" \
    "QTreeView::indicator:enabled:checked" \
    "{" \
    "image: url(:/Images/checkBoxChecked);" \
    "}" \
    "QTreeView::indicator:enabled:checked:hover" \
    "{" \
    "image: url(:/Images/checkBoxCheckedHover);" \
    "}" \
    "QTreeView::indicator:enabled:checked:pressed" \
    "{" \
    "image: url(:/Images/checkBoxCheckedPressed);" \
    "}" \
    "QTableView::indicator:enabled:indeterminate" \
    "{" \
    "image: url(:/Images/checkBoxIndeterminate);" \
    "}" \
    "QTreeView::indicator:enabled:indeterminate:hover" \
    "{" \
    "image: url(:/Images/checkBoxIndeterminateHover);" \
    "}" \
    "QTreeView::indicator:enabled:indeterminate:pressed" \
    "{" \
    "image: url(:/Images/checkBoxIndeterminatePressed);" \
    "}"

    #define buttonColorSelected \
    "QPushButton" \
    "{" \
    "background-color:#%1;" \
    "border-style:outset;" \
    "border:1px solid #7A7A7A;" \
    "}"
};

#endif // STYLE_SHEET_H
