#line 1
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

void main(void) {
    PrimitiveInstance pi = fetch_prim_instance();
    AlphaBatchTask dest_task = fetch_alpha_batch_task(pi.render_task_index);
    AlphaBatchTask src_task = fetch_alpha_batch_task(pi.user_data.x);

    vec2 dest_origin = dest_task.render_target_origin -
                       dest_task.screen_space_origin +
                       src_task.screen_space_origin;

    vec2 local_pos = mix(dest_origin,
                         dest_origin + src_task.size,
                         aPosition.xy);

    vec2 texture_size = vec2(textureSize(sCacheRGBA8, 0));
    vec2 st0 = src_task.render_target_origin / texture_size;
    vec2 st1 = (src_task.render_target_origin + src_task.size) / texture_size;
    vUv = vec3(mix(st0, st1, aPosition.xy), src_task.render_target_layer_index);
    vUvBounds = vec4(st0 + 0.5 / texture_size, st1 - 0.5 / texture_size);

    vOp = pi.sub_index;
    vAmount = float(pi.user_data.y) / 65535.0;

    gl_Position = uTransform * vec4(local_pos, pi.z, 1.0);
}
