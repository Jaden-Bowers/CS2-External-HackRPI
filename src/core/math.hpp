#pragma once

struct view_matrix_t {
    float matrix[4][4];
};

struct vec2_t {
    float x{};
    float y{};
};

struct vec3_t {
    float x{};
    float y{};
    float z{};

    bool world_to_screen(vec2_t& screen_pos, const view_matrix_t& view_matrix, float screen_width, float screen_height) const {
        const float w = view_matrix.matrix[3][0] * x + view_matrix.matrix[3][1] * y + view_matrix.matrix[3][2] * z + view_matrix.matrix[3][3];
        if (w < 0.001f)
            return false;

        float xx = view_matrix.matrix[0][0] * x + view_matrix.matrix[0][1] * y + view_matrix.matrix[0][2] * z + view_matrix.matrix[0][3];
        float yy = view_matrix.matrix[1][0] * x + view_matrix.matrix[1][1] * y + view_matrix.matrix[1][2] * z + view_matrix.matrix[1][3];

        const float inv_w = 1.0f / w;
        xx *= inv_w;
        yy *= inv_w;

        screen_pos.x = (screen_width * 0.5f) + (xx * screen_width * 0.5f);
        screen_pos.y = (screen_height * 0.5f) - (yy * screen_height * 0.5f);
        return true;
    }
};