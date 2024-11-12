#include <stdio.h>
#include <string.h>

#define PAGE_SIZE 256
#define TLB_SIZE 16
#define FRAME_SIZE 256

const int offset_mask = 255;

FILE *backing_store;

typedef struct {
    int offset;
    int n;
    int frame_n;
} Page;

Page tlb[TLB_SIZE];
Page pages[PAGE_SIZE];
char physical_memory[PAGE_SIZE][FRAME_SIZE];

char tlb_len = 0;
int frames_len = 0;
int pages_len = 0;
int processed_pages = 0;

int page_fault_count = 0;
int tlb_hit = 0;

Page get_page(int address) {
    Page current_page;
    current_page.offset = address & offset_mask;
    current_page.n = (address >> 8) & offset_mask;

    return current_page;
}

int read_from_file(Page *current_page) {
    if (frames_len >= PAGE_SIZE || pages_len >= PAGE_SIZE) {
        return 1;
    }

    fseek(backing_store, current_page->n * FRAME_SIZE, SEEK_SET);
    fread(physical_memory[frames_len], sizeof(char), FRAME_SIZE, backing_store);

    pages[pages_len].n = current_page->n;
    pages[pages_len].frame_n = frames_len;
    pages_len++;

    return frames_len++;
}

void insert_tlb(Page *current_page) {
    if (tlb_len == TLB_SIZE) {
        memmove(&tlb[0], &tlb[1], sizeof(Page) * (TLB_SIZE - 1));
        tlb_len--;
    }
    tlb[tlb_len++] = *current_page;
}

int get_frame(Page *current_page) {
    char hit = 0;

    for (int i = tlb_len; i >= 0; --i) {
        if (tlb[i].n == current_page->n) {
            current_page->frame_n = tlb[i].frame_n;
            ++tlb_hit;
            hit = 1;
            break;
        }
    }

    for (int i = 0; i < pages_len && !hit; ++i) {
        if (pages[i].n == current_page->n) {
            current_page->frame_n = pages[i].frame_n;
            hit = 1;
            break;
        }
    }

    if (!hit) {
        current_page->frame_n = read_from_file(current_page);
        ++page_fault_count;
    }

    insert_tlb(current_page);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2 || !freopen(argv[1], "r", stdin)) {
        printf("Input reading error\n");
        return 1;
    }

    backing_store = fopen("BACKING_STORE.bin", "rb");
    if (backing_store == NULL) {
        printf("BACKING_STORE.bin reading error\n");
        return 1;
    }

    freopen("check.txt", "w", stdout);

    int log_address = 0;

    while (scanf("%d", &log_address) == 1) {
        Page current_page = get_page(log_address);

        get_frame(&current_page);

        printf("Virtual address: %d Physical address: %d Value: %d\n",
               (current_page.n << 8) + current_page.offset,
               (current_page.frame_n << 8) | current_page.offset,
               physical_memory[current_page.frame_n][current_page.offset]);
        processed_pages++;
    }

    printf("\nPage fault frequency = %0.4f%%\n", page_fault_count * 100. / processed_pages);
    printf("TLB frequency = %0.4f%%\n", tlb_hit * 100. / processed_pages);

    return 0;
}
