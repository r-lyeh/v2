// -----------------------------------------------------------------------------
// 2D quad drawing

API void quad_render( texture_t texture, vec2 tex_start, vec2 tex_end, unsigned rgba, vec2 start, vec2 end, bool blend );
API void quad_render_id( int texture_type, int texture_id, vec2 dims, vec2 tex_start, vec2 tex_end, unsigned rgba, vec2 start, vec2 end, bool blend );

#if CODE

void quad_render_id( int texture_type, int texture_id, vec2 dims, vec2 tex_start, vec2 tex_end, unsigned rgba, vec2 start, vec2 end, bool blend ) {
    static renderstate_t rect_rs;
    do_once {
        rect_rs = renderstate();
        rect_rs.depth_test_enabled = false;
        rect_rs.blend_enabled = true;
        rect_rs.blend_src = GL_SRC_ALPHA;
        rect_rs.blend_dst = GL_ONE_MINUS_SRC_ALPHA;
        rect_rs.front_face = GL_CW;
    }
    rect_rs.blend_enabled = blend;
    static int program = -1, vbo = -1, vao = -1, u_tint = -1, u_has_tex = -1, u_app_width = -1, u_app_height = -1;
    float gamma = 1;
    vec2 dpi = ifdef(osx, window_dpi(), vec2(1,1));
    if( program < 0 ) {
        const char* vs = file_read("shaders/rect2d_vs.glsl", 0);
        const char* fs = file_read("shaders/rect2d_fs.glsl", 0);

        program = shader(vs, fs, "", "fragcolor" , NULL);
        ASSERT(program > 0);
        u_tint = glGetUniformLocation(program, "u_tint");
        u_has_tex = glGetUniformLocation(program, "u_has_tex");
        u_app_width = glGetUniformLocation(program, "u_app_width");
        u_app_height = glGetUniformLocation(program, "u_app_height");
        glGenVertexArrays( 1, (GLuint*)&vao );
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    }

    start = mul2(start, dpi);
    end = mul2(end, dpi);

    renderstate_apply(&rect_rs);

    glUseProgram( program );

    glBindVertexArray( vao );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( texture_type, texture_id );

    glUniform1i(u_has_tex, (texture_id != 0));
    glUniform1f(u_app_width, (float)app_width());
    glUniform1f(u_app_height, (float)app_height());

    vec4 rgbaf = vec4(((rgba>>0)&255)/255., ((rgba>>8)&255)/255.,((rgba>>16)&255)/255.,((rgba>>24)&255)/255.);
    glUniform4fv(u_tint, 1, &rgbaf.x);

    // render full texture
    if (tex_end.x == 0.0f && tex_end.y == 0.0f) {
        tex_end.x = dims.x;
        tex_end.y = dims.y;
    }

    // normalize texture regions
    if (texture_id != 0) {
        tex_start.x /= dims.x;
        tex_start.y /= dims.y;
        tex_end.x /= dims.x;
        tex_end.y /= dims.y;
    }

    GLfloat vertices[] = {
        // Positions      // UVs
        start.x, start.y, tex_start.x, tex_start.y,
        end.x, start.y,   tex_end.x, tex_start.y,
        end.x, end.y,     tex_end.x, tex_end.y,
        start.x, start.y, tex_start.x, tex_start.y,
        end.x, end.y,     tex_end.x, tex_end.y,
        start.x, end.y,   tex_start.x, tex_end.y
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glDrawArrays( GL_TRIANGLES, 0, 6 );
    profile_incstat("Render.num_drawcalls", +1);
    profile_incstat("Render.num_triangles", +2);

    glBindTexture( texture_type, 0 );
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray( 0 );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram( 0 );
}

void quad_render( texture_t texture, vec2 tex_start, vec2 tex_end, unsigned rgba, vec2 start, vec2 end, bool blend ) {
    quad_render_id(texture.flags & TEXTURE_ARRAY ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture.id, vec2(texture.w, texture.h), tex_start, tex_end, rgba, start, end, blend);
}

#endif
