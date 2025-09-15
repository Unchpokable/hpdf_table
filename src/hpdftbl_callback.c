/**
 * @file
 * @brief    Routines for plain and dynamic callback function.
 *
 * All functions ending with `_cb` are used to specify standard callback functions which stores
 * a function pointer bounded at compile time.
 *
 * @author   Johan Persson (johan162@gmail.com)
 *
 * Copyright (C) 2022 Johan Persson
 *
 * @see LICENSE
 *
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#if !(defined _WIN32 || defined __WIN32__)

#include <unistd.h>

#endif

#include <string.h>
#include <iconv.h>
#include <hpdf.h>


#include "hpdftbl.h"


/**
 * @brief Set table content callback
 *
 * This callback gets called for each cell in the
 * table and the returned string will be used as the content. The string
 * will be duplicated so it is safe for a client to reuse the string space.
 * If NULL is returned from the callback then the content will be set to the
 * content specified with the direct content setting.
 * The callback function will receive the Table tag and the row and column for the
 * cell the callback is made for.
 * @param t Table handle
 * @param cb Callback function
 * @return -1 for error , 0 otherwise
 *
 * @see hpdftbl_set_cell_content_cb()
 */
int
hpdftbl_set_content_cb(hpdftbl_t t, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->content_cb = cb;
    return 0;
}

/**
 * @brief Set cell content callback.
 *
 * Set a content callback for an individual cell. This will override the table content
 * callback. The callback function will receive the Table tag and the row and column for the
 * cell the callback is made for.
 * @param t Table handle
 * @param cb Callback function
 * @param r Cell row
 * @param c Cell column
 * @return -1 on failure, 0 otherwise
 *
 * @see hpdftbl_set_content_cb()
 */
int
hpdftbl_set_cell_content_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c))
        return -1;

    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    cell->content_cb = cb;
    return 0;
}

/**
 * @brief Set cell label callback
 *
 * Set a label callback for an individual cell. This will override the table label
 * callback. The callback function will receive the Table tag and the row and column for the
 * cell the callback is made for.
 * @param t Table handle
 * @param cb Callback function
 * @param r Cell row
 * @param c Cell column
 * @return -1 on failure, 0 otherwise
 *
 * @see hpdftbl_set_label_cb()
 */
int
hpdftbl_set_cell_label_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c))
        return -1;

    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    cell->label_cb = cb;
    return 0;
}

/**
 * @brief Set cell canvas callback
 *
 * Set a canvas callback for an individual cell. This will override the table canvas
 * callback. The canvas callback is called with arguments that give the bounding box for the
 * cell. In that way a callback function may draw arbitrary graphic in the cell.
 * The callback is made before the cell border and content is drawn making
 * it possible to for example add a background color to individual cells.
 * The callback function will receive the Table tag, the row and column,
 * the x, y position of the lower left corner of the table and the width
 * and height of the cell.
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdftbl_canvas_callback_t
 * @see hpdftbl_set_canvas_cb()
 */
int
hpdftbl_set_cell_canvas_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_canvas_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    if (!chktbl(t, r, c))
        return -1;

    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];

    // If this cell is part of another cells spanning then
    // indicate this as an error
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }

    cell->canvas_cb = cb;
    return 0;
}

/**
 * @brief Set table label callback
 *
 * Set label callback. This callback gets called for each cell in the
 * table and the returned string will be used as the label. The string
 * will be duplicated so it is safe for a client to reuse the string space.
 * If NULL is returned from the callback then the label will be set to the
 * content specified with the direct label setting.
 * The callback function will receive the Table tag and the row and column
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdftbl_content_callback_t
 * @see hpdftbl_set_cell_label_cb()
 */
int
hpdftbl_set_label_cb(hpdftbl_t t, hpdftbl_content_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->label_cb = cb;
    return 0;
}

/**
 * @brief Set table post processing callback
 *
 * This is an optional post processing callback for anything in general to do
 * after the table has been constructed.
 * The callback happens after the table has been fully constructed and just
 * before it is stroked.
 *
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 * @see hpdftbl_callback_t
 */
int
hpdftbl_set_post_cb(hpdftbl_t t, hpdftbl_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->post_cb = cb;
    return 0;
}

/**
 * @brief Set cell canvas callback
 *
 * Set cell canvas callback. This callback gets called for each cell in the
 * table. The purpose is to allow the client to add dynamic content to the
 * specified cell.
 * The callback is made before the cell border and content is drawn making
 * it possible to for example add a background color to individual cells.
 * The callback function will receive the Table tag, the row and column,
 * the x, y position of the lower left corner of the table and the width
 * and height of the cell.
 * To set the canvas callback only for a specific cell use the
 * hpdftbl_set_cell_canvas_cb() function
 * @param t Table handle
 * @param cb Callback function
 * @return -1 on failure, 0 otherwise
 *
 * @see hpdftbl_set_cell_canvas_cb()
 */
int
hpdftbl_set_canvas_cb(hpdftbl_t t, hpdftbl_canvas_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->canvas_cb = cb;
    return 0;
}

/**
 * @brief Set cell specific callback to specify cell content style.
 *
 * Set callback to format the style for the specified cell
 * @param t Table handle
 * @param r Cell row
 * @param c Cell column
 * @param cb Callback function
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_ontent_style_cb()
 */
int
hpdftbl_set_cell_content_style_cb(hpdftbl_t t, size_t r, size_t c, hpdftbl_content_style_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    chktbl(t, r, c);
    // If this cell is part of another cells spanning then
    // indicate this as an error
    hpdftbl_cell_t *cell = &t->cells[_HPDFTBL_IDX(r, c)];
    if (cell->parent_cell) {
        _HPDFTBL_SET_ERR(t, -1, r, c);
        return -1;
    }
    cell->style_cb = cb;
    return 0;
}

/**
 * @brief Set callback to specify cell content style
 *
 * Set callback to format the style for cells in the table. If a cell has its own content style
 * callback that callback will override the generic table callback.
 * @param t Table handle
 * @param cb Callback function
 * @return 0 on success, -1 on failure
 *
 * @see hpdftbl_set_cell_content_style_cb()
 */
int
hpdftbl_set_content_style_cb(hpdftbl_t t, hpdftbl_content_style_callback_t cb) {
    _HPDFTBL_CHK_TABLE(t);
    t->content_style_cb = cb;
    return 0;
}

/* EOF */
