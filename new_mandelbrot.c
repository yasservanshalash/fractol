#include "MLX42/include/MLX42/MLX42.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 800
#define MAX_ITER 75

typedef struct {
    mlx_image_t* img;
    mlx_t* mlx;
    double zoom;
    double offsetX;
    double offsetY;
} t_mandelbrot;

static void ft_error(const char* message) {
    fprintf(stderr, "%s: %s\n", message, mlx_strerror(mlx_errno));
    exit(EXIT_FAILURE);
}

uint32_t get_color(int iter) {
    if (iter == MAX_ITER) return 0x000000;
    double t = (double)iter / MAX_ITER;
    uint8_t value = 255 * (1 - t);
    return (value << 16) | (value << 8) | value;
}

int mandelbrot(double x, double y) {
    double zx = 0, zy = 0, cx = x, cy = y, tmp;
    int iter = 0;
    while (zx * zx + zy * zy < 4 && iter < MAX_ITER) {
        tmp = zx * zx - zy * zy + cx;
        zy = 2 * zx * zy + cy;
        zx = tmp;
        iter++;
    }
    return iter;
}

void draw_mandelbrot(t_mandelbrot *data) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double jx = (x - WIDTH / 2.0) / data->zoom + data->offsetX;
            double jy = (y - HEIGHT / 2.0) / data->zoom + data->offsetY;
            int iter = mandelbrot(jx, jy);
            uint32_t color = get_color(iter);
            mlx_put_pixel(data->img, x, y, color);
        }
    }
    mlx_image_to_window(data->mlx, data->img, 0, 0);
}

void hook(void *param) {
    t_mandelbrot *data = (t_mandelbrot *)param;
    int mouse_x, mouse_y;
    mlx_get_mouse_pos(data->mlx, &mouse_x, &mouse_y);

    // Calculate the position in the Mandelbrot set that the mouse points to
    double mouse_re = (mouse_x - WIDTH / 2.0) / data->zoom + data->offsetX;
    double mouse_im = (mouse_y - HEIGHT / 2.0) / data->zoom + data->offsetY;

    // Zoom in
    if (mlx_is_key_down(data->mlx, MLX_KEY_EQUAL)) {
        data->zoom *= 1.1;
        data->offsetX += (mouse_re - data->offsetX) * (1 - 1 / 1.1);
        data->offsetY += (mouse_im - data->offsetY) * (1 - 1 / 1.1);
        draw_mandelbrot(data);
    }
    // Zoom out
    if (mlx_is_key_down(data->mlx, MLX_KEY_MINUS)) {
        data->zoom /= 1.1;
        data->offsetX -= (mouse_re - data->offsetX) * (1 - 1.1);
        data->offsetY -= (mouse_im - data->offsetY) * (1 - 1.1);
        draw_mandelbrot(data);
    }
    // Movement controls remain unchanged
    if (mlx_is_key_down(data->mlx, MLX_KEY_LEFT)) {
        data->offsetX -= 30 / data->zoom;
        draw_mandelbrot(data);
    }
    if (mlx_is_key_down(data->mlx, MLX_KEY_RIGHT)) {
        data->offsetX += 30 / data->zoom;
        draw_mandelbrot(data);
    }
    if (mlx_is_key_down(data->mlx, MLX_KEY_UP)) {
        data->offsetY -= 30 / data->zoom;
        draw_mandelbrot(data);
    }
    if (mlx_is_key_down(data->mlx, MLX_KEY_DOWN)) {
        data->offsetY += 30 / data->zoom;
        draw_mandelbrot(data);
    }
}



int main(void) {
    mlx_t* mlx = mlx_init(WIDTH, HEIGHT, "Zoomable Mandelbrot Set", true);
    if (!mlx)
        ft_error("Failed to initialize MLX");

    t_mandelbrot data = {
        .img = mlx_new_image(mlx, WIDTH, HEIGHT),
        .zoom = WIDTH / 2.5,  // Adjusted zoom factor for a broader view
        .offsetX = -0.75,     // Centered slightly to the left to include main and smaller sets
        .offsetY = 0.0,
        .mlx = mlx
    };
    if (!data.img)
        ft_error("Failed to create image");

    draw_mandelbrot(&data);
    mlx_loop_hook(mlx, hook, &data);
    mlx_loop(mlx);
    mlx_terminate(mlx);
    return EXIT_SUCCESS;
}
