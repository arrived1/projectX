#pragma once


int mouse_x, mouse_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0;
float translate_z = -3.0;

// parametry widoku - kamery
int ox, oy;
int buttonState = 0;
float camera_trans[] = { 0, 0, -3 };
float camera_rot[] = { 0, 0, 0 };
float camera_trans_lag[] = { 0, 0, -3 };
float camera_rot_lag[] = { 0, 0, 0 };
const float inertia = 0.1;
float modelView[16];


void mouse(int button, int state, int x, int y)
{
    /*
    if (state == GLUT_DOWN) {
    mouse_buttons |= 1 << button;
    }
    else if (state == GLUT_UP) {
    mouse_buttons = 0;
    }

    mouse_x = x;
    mouse_y = y;
    glutPostRedisplay();
    */

    if (state == GLUT_DOWN)
        buttonState |= 1 << button;
    else if (state == GLUT_UP)
        buttonState = 0;

    int mods = glutGetModifiers();
    if (mods & GLUT_ACTIVE_SHIFT)
    {
        buttonState = 2;
    }
    else if (mods & GLUT_ACTIVE_CTRL)
    {
        buttonState = 3;
    }

    ox = x; oy = y;

    glutPostRedisplay();
}



void motion(int x, int y)
{
    /*
    float dx, dy;
    dx = x - mouse_x;
    dy = y - mouse_y;

    if (mouse_buttons & 1) {
    rotate_x += dy * 0.2;
    rotate_y += dx * 0.2;
    }
    else if (mouse_buttons & 4) {
    translate_z += dy * 0.01;
    }

    mouse_x = x;
    mouse_y = y;
    */

    float dx = x - ox;
    float dy = y - oy;

    if (buttonState == 3)
    {
        // left+middle = zoom
        camera_trans[2] += (dy / 100.0) * 0.5 * fabs(camera_trans[2]);
    }
    else if (buttonState & 2)
    {
        // middle = translate
        camera_trans[0] += dx / 10.0;
        camera_trans[1] -= dy / 10.0;
    }
    else if (buttonState & 1)
    {
        // left = rotate
        camera_rot[0] += dy / 5.0;
        camera_rot[1] += dx / 5.0;
    }

    ox = x; oy = y;
}

void myCamera()
{
    // view transform
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    for (int c = 0; c < 3; ++c)
    {
        camera_trans_lag[c] += (camera_trans[c] - camera_trans_lag[c]) * inertia;
        camera_rot_lag[c] += (camera_rot[c] - camera_rot_lag[c]) * inertia;
    }
    glTranslatef(camera_trans_lag[0], camera_trans_lag[1], camera_trans_lag[2]);
    glRotatef(camera_rot_lag[0], 1.0, 0.0, 0.0);
    glRotatef(camera_rot_lag[1], 0.0, 1.0, 0.0);

    glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
}

